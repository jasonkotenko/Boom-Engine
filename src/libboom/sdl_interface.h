/*
	SDL Interface class
*/

#ifndef _SDL_INTERFACE_H
#define _SDL_INTERFACE_H

#include "interface.h"

namespace Boom
{
	namespace Interface
	{
		class SDLInterface: public BaseInterface
		{
			public:
				SDLInterface(unsigned int width = 640, unsigned int height = 480);
				~SDLInterface();
				
				void start();
				void queue_flip();
				
			private:
				void shutdown(void *args);
				bool running;
		};
	}
}

#endif
