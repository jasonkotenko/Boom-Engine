/*
	Logging Facility
	================
		A generic logging facility. Default log output is to stdout.
		
		Usage Example
		-------------
			#include "log.h"
			
			LOG_INFO << "This is some info!" << endl;
			// Do some stuff here...
			LOG_WARNING << "Whoops, something isn't right!" << endl;
		
		Log Message Types
		-----------------
			LOG_DEBUG    - Simple debug messages
			LOG_INFO     - Informational message
			LOG_WARNING  - Problem that will not cause execution to stop
			LOG_ERROR    - Problem that will cause execution to stop,
						   but can be fixed
			LOG_CRITICAL - Problem that will cause execution to stop,
						   but cannot be fixed (should exit after this!)
		
		Copyright 2006 Daniel G. Taylor
*/

#ifndef _LOG_H
#define _LOG_H

#include <iostream>

using namespace std;

/*
	Below are macros for printing log messages easily. The message format
	is "filename [line]: TYPE message".
*/
#define LOG_DEBUG \
	if (Boom::Log::level <= Boom::Log::LEVEL_DEBUG) \
	*(Boom::Log::output) << __FILE__ << " [" << __LINE__ << "]: DEBUG "

#define LOG_INFO \
	if (Boom::Log::level <= Boom::Log::LEVEL_INFO) \
	*(Boom::Log::output) << __FILE__ << " [" << __LINE__ << "]: INFO "

#define LOG_WARNING \
	if (Boom::Log::level <= Boom::Log::LEVEL_WARNING) \
	*(Boom::Log::output) << __FILE__ << " [" << __LINE__ << "]: WARNING "

#define LOG_ERROR \
	if (Boom::Log::level <= Boom::Log::LEVEL_ERROR) \
	*(Boom::Log::output) << __FILE__ << " [" << __LINE__ << "]: ERROR "

#define LOG_CRITICAL \
	if (Boom::Log::level <= Boom::Log::LEVEL_CRITICAL) \
	*(Boom::Log::output) << __FILE__ << " [" << __LINE__ << "]: CRITICAL "

namespace Boom
{
	/// The logging system.
	/*!
		Provides facilities to output log messages.
		
		Macros are defined so the functions below don't need to be used directly,
		instead one can do something like
		
		\code
		#include "boom/log.h"
		
		void myfunc()
		{
			LOG_DEBUG << "This is a debug message!" << endl;
		}
		\endcode
	*/
	namespace Log
	{
		//! Defines log output verbosity levels.
		enum LogLevel
		{
			LEVEL_DEBUG,	//!< Debug messages
			LEVEL_INFO,		//!< General informational messages
			LEVEL_WARNING,	//!< Warning messages
			LEVEL_ERROR,	//!< Recoverable error messages
			LEVEL_CRITICAL,	//!< Non-recoverable error messages
			LEVEL_DISABLED	//!< No logging
		};
	
		extern LogLevel level;
		extern ostream *output;
		
		//! Initialize the logging system
		void init();
		
		//! Clean up memory before shutdown
		void cleanup();
		
		//! Set the output stream (e.g. cout, a file, etc)
		void set_output(ostream *new_output);
		
		/// Set the minimum output level.
		/*!
			Set the minimum output level, e.g. specifying LEVEL_INFO will
			only print messages that have a level of INFO or higher,
			ignoring messages with a level of DEBUG.
		*/
		void set_level(LogLevel new_level);
	}
}

#endif
