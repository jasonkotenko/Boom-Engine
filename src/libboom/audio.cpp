#include "audio.h"

namespace Boom
{
	namespace Audio
	{
		Mix_Music *music = NULL;
	
		void init()
		{
			SDL_Init(SDL_INIT_AUDIO);
			Mix_OpenAudio(22050, AUDIO_S16, 2, 4096);
			
		}
		
		void cleanup()
		{
			Mix_HaltMusic();
			Mix_CloseAudio();
		}
		
		void play_music(string filename, int loops)
		{
			if (music)
			{
				Mix_HaltMusic();
				Mix_FreeMusic(music);
			}
			
			music = Mix_LoadMUS(filename.c_str());
			Mix_PlayMusic(music, loops);
		}
	}
}
