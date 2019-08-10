#pragma once
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <time.h>
#include <array>
#include <vector>
using namespace std;



template <unsigned input_count, unsigned output_count>
class supervised_ai : public map<array<double, input_count>, array<double, output_count>>
{
public:
	typedef array<double, input_count> input_data;
	typedef array<double, output_count> output_data;

	vector<double> priorities;
	double priority_sum;
	unsigned point_limit;

	supervised_ai(unsigned _point_limit) : point_limit(_point_limit)
	{
	}

	void remove_random_point()
	{
		auto it = this->begin();
		int index = rand() % this->size();
		while (index)
		{
			it++;
			index--;
		}
		this->erase(it);
	}
	
	void train(const input_data& input, output_data& expected_output)
	{
		(*this)[input] = expected_output;
		while (this->size() > point_limit) remove_random_point();
	}

	void get_output(const input_data& input, output_data& output)
	{
		priorities.clear();
		priorities.reserve(this->size());
		priority_sum = 0.0;
		for (auto& point : *this)
		{
			double distance2 = 0.0;
			for (unsigned i = 0; i < input_count; i++)
			{
				distance2 += pow(point.first[i] - input[i], 2.0);
			}
			if (distance2 == 0.0)
			{
				output = point.second;
				return;
			}
			double priority = exp(-distance2 * this->size()) / distance2;
			priorities.push_back(priority);
			priority_sum += priority;
		}

		output.fill(0.0);
		auto point_it = this->begin();
		auto priority_it = priorities.begin();
		for (; point_it != this->end(); point_it++, priority_it++)
		{
			double factor = (*priority_it) / priority_sum;
			for (unsigned o = 0; o < output_count; o++)
			{
				output[o] += point_it->second[o] * factor;
			}
		}
	}
};



template <unsigned state_data_count, unsigned action_data_count>
class reward_model : public supervised_ai<state_data_count + action_data_count, 1>
{
public:
	typedef array<double, state_data_count> state_data;
	typedef array<double, action_data_count> action_data;
	typedef array<double, state_data_count + action_data_count> input_data;
	typedef array<double, 1> output_data;

private:
	input_data _input;
	double* _action = _input.data() + state_data_count;

public:
	output_data last_reward;

	static void copy_state_to_input(const state_data& state, input_data& input)
	{
		memcpy(input.data(), state.data(), sizeof(double) * state_data_count);
	}

	static void copy_action_to_input(const action_data& action, input_data& input)
	{
		memcpy(input.data() + state_data_count, action.data(), sizeof(double) * action_data_count);
	}

	static void explore(action_data& output_action)
	{
		for (unsigned o = 0; o < action_data_count; o++)
		{
			output_action[o] = double(rand()) / RAND_MAX;
		}
	}

	static double calculate_dynamism(const state_data& prev_state, const state_data& current_state)
	{
		double reward = 0.0;
		for (unsigned s = 0; s < state_data_count; s++)
		{
			double difference = current_state[s] - prev_state[s];
			reward += difference * difference;
		}
		return reward;
	}

	static double calculate_droop(const state_data& prev_state, const state_data& current_state)
	{
		return -calculate_dynamism(prev_state, current_state);
	}

	reward_model(unsigned _point_limit) : supervised_ai<state_data_count + action_data_count, 1>(_point_limit)
	{
	}

	void train_model(const state_data& prev_state, const action_data& action, double reward)
	{
		copy_state_to_input(prev_state, _input);
		copy_action_to_input(action, _input);
		last_reward[0] = reward;
		this->train(_input, last_reward);
	}

	void train_model_rewarding_dynamism(const state_data& prev_state, const action_data& action, const state_data& current_state)
	{
		train_model(prev_state, action, calculate_dynamism(prev_state, current_state));
	}

	void train_model_rewarding_droop(const state_data& prev_state, const action_data& action, const state_data& current_state)
	{
		train_model(prev_state, action, calculate_droop(prev_state, current_state));
	}

	bool find_best_action(const state_data& state, action_data& output_action)
	{
		if (this->empty()) return false;
		
		auto max_it = this->end();
		copy_state_to_input(state, _input);
		for (auto it = this->begin(); it != this->end(); it++)
		{
			memcpy(_action, it->first.data() + state_data_count, sizeof(double) * action_data_count);
			output_data reward;
			this->get_output(_input, reward);
			if (max_it == this->end())
			{
				max_it = it;
				last_reward = reward;
			}
			else if (reward[0] > last_reward[0])
			{
				max_it = it;
				last_reward = reward;
			}
		}
		
		memcpy(output_action.data(), max_it->first.data() + state_data_count, sizeof(double) * action_data_count);
		return true;
	}

	void find_best_action(const state_data& state, double min_time_seconds, action_data& output_action)
	{
		clock_t finish_time = clock() + min_time_seconds * CLOCKS_PER_SEC;
		double step = 1.0;
		double half_step = 0.5;
		output_action[0] = -1.0;
		copy_state_to_input(state, _input);
		while (clock() < finish_time)
		{
			for (unsigned a = 0; a < action_data_count; a++)
			{
				_action[a] = half_step;
			}

			while (clock() < finish_time)
			{
				output_data reward;
				this->get_output(_input, reward);
				if (output_action[0] == -1.0)
				{
					memcpy(output_action.data(), _action, sizeof(double) * action_data_count);
					last_reward = reward;
				}
				else if (reward[0] > last_reward[0])
				{
					memcpy(output_action.data(), _action, sizeof(double) * action_data_count);
					last_reward = reward;
				}

				unsigned a = 0;
				bool exit = false;
				while (true)
				{
					_action[a] += step;
					if (_action[a] > 1.0)
					{
						_action[a] = half_step;
						a++;
						if (a == action_data_count)
						{
							exit = true;
							break;
						}
					}
					else break;
				}
				if (exit) break;
			}

			step = half_step;
			half_step *= 0.5;
		}
	}
};