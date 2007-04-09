#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <string>
#include "SDL.h"
#include "SDL_mixer.h"

using namespace std;

namespace Boom
{
	//! Audio Namespace
	/*!
		Initialize and cleanup the audio subsystem and provide a way for other
		parts of the game engine to produce sound effects and music.
	*/
	namespace Audio
	{
		const int LOOP_FOREVER = -1;
	
		//! Initialize the audio engine
		void init();
		//! Cleanup the audio engine
		void cleanup();
		
		//! Play a background song
		void play_music(string filename, int loops = LOOP_FOREVER);
	}
}

#endif
