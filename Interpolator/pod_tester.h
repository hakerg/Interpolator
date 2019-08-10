#pragma once
#include "AI.h"
#include "allegro5/allegro.h"
#include "allegro5/allegro_primitives.h"
#include "pod.h"

class pod_tester
{
public:
	reward_model<9, 3> ai;
	reward_model<9, 3>::state_data ai_state;
	reward_model<9, 3>::action_data ai_action;
	pod player;
	double point_pos[2] = { 0.25, 0.25 };

	pod_tester() : ai(1000)
	{
	}

	double distance_to_point(const pod& source)
	{
		return sqrt(pow(source.pos[0] - point_pos[0], 2.0) + pow(source.pos[1] - point_pos[1], 2.0));
	}

	void set_ai_state(const pod& source)
	{
		ai_state[0] = source.pos[0];
		ai_state[1] = source.pos[1];
		ai_state[2] = sin(source.rot);
		ai_state[3] = cos(source.rot);
		ai_state[4] = source.speed[0];
		ai_state[5] = source.speed[1];
		ai_state[6] = source.rot_speed;
		ai_state[7] = point_pos[0];
		ai_state[8] = point_pos[1];
	}

	void loop()
	{
		al_clear_to_color(al_map_rgb(0, 0, 0));

		ai.explore(ai_action);
		pod explorer = player;
		explorer.move(ai_action[0], ai_action[1], ai_action[2]);
		ai.train_model(ai_state, ai_action, 1.0 / distance_to_point(explorer));

		set_ai_state(player);
		ai.find_best_action(ai_state, 30, ai_action);
		player.move(ai_action[0], ai_action[1], ai_action[2]);

		if (distance_to_point(player) <= 0.03)
		{
			point_pos[0] = double(rand()) / RAND_MAX;
			point_pos[1] = double(rand()) / RAND_MAX;
		}

		al_draw_filled_circle(point_pos[0] * 600, point_pos[1] * 600, 6, al_map_rgb(255, 0, 0));
		al_draw_filled_circle(player.pos[0] * 600, player.pos[1] * 600, 6, al_map_rgb(0, 255, 0));
		al_draw_line(player.pos[0] * 600, player.pos[1] * 600,
			player.pos[0] * 600 + cos(player.rot) * 12, player.pos[1] * 600 + sin(player.rot) * 12,
			al_map_rgb(255, 255, 255), 2.0f);

		al_flip_display();

		al_rest(0.01);
	}
};

