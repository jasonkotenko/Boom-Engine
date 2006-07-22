/*
	SDL Interface
*/

#include "sdl_interface.h"
#include "event.h"
#include "state.h"

#include <SDL.h>

namespace Boom
{
	namespace Interface
	{
		SDLInterface::SDLInterface(unsigned int width, unsigned int height)
		{
			SDL_Init(SDL_INIT_VIDEO);
			SDL_Surface *screen;
		    
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
			
			screen = SDL_SetVideoMode(width, height, 0, SDL_HWSURFACE | SDL_OPENGL);
			
			init_gl();
			
			resize(width, height);
			
			Event::connect(EVENT_QUIT, this, &SDLInterface::shutdown);
			
			running = true;
		}
		
		SDLInterface::~SDLInterface()
		{
			shutdown(NULL);
		}
		
		void SDLInterface::shutdown(void *args)
		{
			running  = false;
		}
		
		void SDLInterface::start()
		{
			SDL_Event event;
			
			while (running)
			{
				Event::process();
				
				while(SDL_PollEvent(&event))
				{
					switch(event.type)
					{
						case SDL_QUIT:
							Event::post(EVENT_QUIT, NULL, Event::PRIORITY_HIGH);
							return;
						case SDL_KEYDOWN:
							if (State::current)
							{
								State::current->key_pressed(int(event.key.keysym.sym));
							}
							Event::post(EVENT_KEY_DOWN, reinterpret_cast<void*>(&event.key.keysym.sym), Event::PRIORITY_HIGH);
							break;
					}
				}
				
				State::update();
				
				draw();
			}
		
			SDL_Quit();
		}
		
		void SDLInterface::queue_flip()
		{
			SDL_GL_SwapBuffers();
		}
	}
}
