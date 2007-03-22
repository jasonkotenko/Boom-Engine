/*
	Boom Engine Implementation
*/

#include "boom.h"

namespace Boom
{
	const float VERSION = 0.1;
	const char *COMPILE_DATE = __DATE__;
	const char *COMPILE_TIME = __TIME__;

	void init(int &argc, char *argv[])
	{
		srand(time(NULL));
		glutInit(&argc, argv);
		Log::init();
		VirtualFS::init();
		Event::init();
		State::init();
		Audio::init();
	}
	
	void cleanup()
	{
		LOG_INFO << "Shutting down..." << endl;
		Audio::cleanup();
		State::cleanup();
		Event::cleanup();
		VirtualFS::cleanup();
		Log::cleanup();
	}
}
