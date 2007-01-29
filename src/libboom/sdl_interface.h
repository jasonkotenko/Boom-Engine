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
		/// An interface built on SDL
		/*!
			An interface that uses the Simple Directmedia Layer to display a
			window, handle input, etc.
		*/
		class SDLInterface: public BaseInterface
		{
			public:
				SDLInterface(unsigned int width = 640, unsigned int height = 480);
				~SDLInterface();
				
				//! Start the main loop
				void start();
				//! Queue a flip of the drawing buffer
				void queue_flip();
				
			private:
				//! Shutdown SDL cleanly
				void shutdown(void *args);
				//! Whether or not we are currently running
				bool running;
		};
	}
}

#endif
