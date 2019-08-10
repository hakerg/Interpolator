// Interpolator.cpp : Definiuje punkt wejścia dla aplikacji.
//

#include "framework.h"
#include "Interpolator.h"
#include "graph_tester.h"
#include "_2d_tester.h"
#include "pod_tester.h"
using namespace std;

ALLEGRO_DISPLAY* display;
ALLEGRO_EVENT_QUEUE* queue;

int main()
{
	al_init();
	al_init_primitives_addon();

	srand(time(NULL));

	display = al_create_display(600, 600);
	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_display_event_source(display));
	ALLEGRO_EVENT event;

	//graph_tester tester;
	//_2d_tester tester;
	pod_tester tester;

	while (true)
	{
		while (al_get_next_event(queue, &event))
		{
			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) return 0;
		}

		tester.loop();
	}
}
