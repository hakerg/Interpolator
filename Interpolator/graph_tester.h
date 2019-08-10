#pragma once
#include "AI.h"
#include "allegro5/allegro.h"
#include "allegro5/allegro_primitives.h"

class graph_tester
{
public:
	supervised_ai<1, 1> ai;
	supervised_ai<1, 1>::input_data ai_input;
	supervised_ai<1, 1>::output_data ai_output;

	graph_tester() : ai(1000)
	{
	}

	void loop()
	{
		while (ai.size() < 100)
		{
			ai_input[0] = double(rand()) / RAND_MAX;
			ai_output[0] = (sin(ai_input[0] * 10.0) + 1.0) * 0.5;
			//ai_output[0] = ai_input[0];
			ai.train(ai_input, ai_output);
		}

		al_clear_to_color(al_map_rgb(0, 0, 0));

		double prev_y = 0.0;
		for (int x = -1; x < 600; x++)
		{
			ai_input[0] = x / 600.0;
			ai.get_output(ai_input, ai_output);
			ai_output[0] *= 600.0;
			al_draw_line(x - 1, prev_y, x, ai_output[0], al_map_rgb(255, 255, 255), 2.0f);
			prev_y = ai_output[0];
		}
		for (auto& point : ai)
		{
			al_draw_circle(point.first[0] * 600.0, point.second[0] * 600.0, 6.0f, al_map_rgb(255, 0, 0), 2.0f);
		}
		al_flip_display();
		
		al_rest(0.1);

		ai.remove_random_point();
	}
};

