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
	/// The state namespace
	/*!
		Handles moving between states within the game engine. Convenience
		functions are provided for modifying the stack and updating/drawing the
		current state (top of the stack).
	*/
	namespace State
	{
		/// A virtual base class for states
		/*!
			A virtual base class that should be inherited when creating new
			states, such as e.g. a menu state, playing state, paused state, etc.
		*/
		class State
		{
			public:
				State();
				virtual ~State() = 0;
				
				//! Update the state (e.g. move objects)
				virtual void update() = 0;
				//! Draw the state (e.g. a menu or scene)
				virtual void draw() = 0;
				
				//! Handle a key press
				virtual void key_pressed(int key) = 0;
				//! Handle a key release
				virtual void key_released(int key) = 0;
				
			protected:
				bool running;
		};
		
		//! A link to the current state (top of the stack)
		extern State *current;
		
		//! Initialize the state stack
		void init();
		//! Cleanup the state stack
		void cleanup();
		
		//! Push a new state onto the stack
		void push(State *state);
		//! Replace the current top state with a new state
		void replace(State *state);
		//! Pop the top of the stack
		void pop(int count = 1);
		//! Clear the stack
		void clear();
		
		//! Update the current state (top of the stack)
		void update();
		//! Draw the current state (top of the stack)
		void draw();
	}
}

#endif
