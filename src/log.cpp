/*
	Logging Facility Implementation
	
	Copyright 2006 Daniel G. Taylor
*/

#include "log.h"

using namespace Boom;

namespace Boom
{
	namespace Log
	{
		LogLevel level = LEVEL_DEBUG;	// current log level
		ostream *output;				// current output stream
	
		void init()
		{
			output = new ostream(cout.rdbuf());
			LOG_INFO << "Initializing logging system..." << endl;
		}
		
		void cleanup()
		{
			// Delete the output stream if needed!
			if (output != NULL)
			{
				delete output;
				output = NULL;
			}
		}
		
		void set_output(const char *filename)
		{
			if (output != NULL)
			{
				delete output;
				output = NULL;
			}
			
			filebuf *fb = new filebuf;
			output = new ostream(fb->open(filename, ios_base::out |
													ios_base::trunc));
		}
		
		void set_level(LogLevel new_level)
		{
			level = new_level;
		}
	}
}
