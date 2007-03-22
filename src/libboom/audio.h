#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <string>
#include "SDL.h"
#include "SDL_mixer.h"

using namespace std;

//! Audio
/*!
	Description
*/
namespace Boom
{
	namespace Audio
	{
		//! Initialize the audio engine
		void init();
		//! Cleanup the audio engine
		void cleanup();
		
		//! Play a background song
		void play_music(string filename);
	}
}

#endif
