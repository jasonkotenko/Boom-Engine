/*
	Game State Manager
	==================
		Manage running and moving between game states such as a menu, playing
		a level, or having the game paused.
		
		Copyright 2006 Daniel G. Taylor
*/

#ifndef _STATE_H
#define _STATE_H

namespace Boom
{
	namespace State
	{
		class State
		{
			public:
				State();
				virtual ~State() = 0;
				
				virtual void update() = 0;
				virtual void draw() = 0;
			
				virtual void key_pressed(int key) = 0;
				virtual void key_released(int key) = 0;
				
			protected:
				bool running;
		};
		
		extern State *current;
		
		void init();
		void cleanup();
		
		void push(State *state);
		void replace(State *state);
		void pop();
		void clear();
		
		void update();
		void draw();
	}
}

#endif
