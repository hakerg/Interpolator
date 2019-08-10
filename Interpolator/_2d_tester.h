#pragma once
#include "AI.h"
#include "allegro5/allegro.h"
#include "allegro5/allegro_primitives.h"

class _2d_tester
{
public:
	supervised_ai<2, 1> ai;
	supervised_ai<2, 1>::input_data ai_input;
	supervised_ai<2, 1>::output_data ai_output;

	_2d_tester() : ai(1000)
	{
	}

	void loop()
	{
		while (ai.size() < 10)
		{
			ai_input[0] = double(rand()) / RAND_MAX;
			ai_input[1] = double(rand()) / RAND_MAX;
			ai_output[0] = double(rand()) / RAND_MAX;
			ai.train(ai_input, ai_output);
		}

		al_clear_to_color(al_map_rgb(0, 0, 0));

		for (int y = 0; y < 600; y++)
		{
			for (int x = 0; x < 600; x++)
			{
				ai_input[0] = x / 600.0;
				ai_input[1] = y / 600.0;
				ai.get_output(ai_input, ai_output);
				al_put_pixel(x, y, al_map_rgb_f(ai_output[0], ai_output[0], ai_output[0]));
			}
		}
		for (auto& point : ai)
		{
			al_draw_circle(point.first[0] * 600.0, point.first[1] * 600.0, 6.0f, al_map_rgb_f(point.second[0], 1.0f, 0.0f), 2.0f);
		}
		al_flip_display();

		al_rest(0.1);

		ai.remove_random_point();
	}
};

