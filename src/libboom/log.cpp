/*
	Logging Facility Implementation
	
	Copyright 2006 Daniel G. Taylor
*/

#include <fstream>
#include "log.h"

using namespace Boom;

namespace Boom
{
	namespace Log
	{
		LogLevel level = LEVEL_INFO;	// current log level
		ostream *output;				// current output stream
	
		void init()
		{
			// Set the default output stream to cout
			output = new ostream(cout.rdbuf());
			LOG_INFO << "Initialized logging system..." << endl;
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
			// Delete the old output stream
			if (output != NULL)
			{
				delete output;
				output = NULL;
			}
			
			// Create a new stream from the filename
			// TODO: Use VirtualFS here???
			filebuf *fb = new filebuf;
			output = new ostream(fb->open(filename, ios_base::out |
													ios_base::trunc));
		}
		
		void set_level(LogLevel new_level)
		{
			/*
				Set the current minimum log level of which messages should be
				printed.
			*/
			level = new_level;
		}
	}
}
