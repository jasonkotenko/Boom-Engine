/*
	State Manager Implementation
*/

#include <stack>
#include <cstdlib>

#include "state.h"

using namespace std;

namespace Boom
{
	namespace State
	{
		State *current;
		stack <State*> state_stack;
		
		State::State()
		{
			running = true;
		}
		
		State::~State()
		{
		
		}
		
		void init()
		{
			current = NULL;
		}
		
		void cleanup()
		{
			while(state_stack.size() > 0)
			{
				current = state_stack.top();
				state_stack.pop();
				delete current;
				current = NULL;
			}
		}
		
		void push(State *state)
		{
			state_stack.push(state);
			current = state;
		}
		
		void replace(State *state)
		{
			state_stack.pop();
			delete current;
			state_stack.push(state);
			current = state;
		}
		
		void pop()
		{
			state_stack.pop();
			delete current;
			if (state_stack.size() > 0)
			{
				current = state_stack.top();
			}
			else
			{
				current = NULL;
			}
		}
		
		void clear()
		{
			while(state_stack.size() > 0)
			{
				current = state_stack.top();
				state_stack.pop();
				delete current;
				current = NULL;
			}
		}
		
		void update()
		{
			if (current != NULL)
			{
				current->update();
			}
		}
		
		void draw()
		{
			if (current != NULL)
			{
				current->draw();
			}
		}
	}
}
