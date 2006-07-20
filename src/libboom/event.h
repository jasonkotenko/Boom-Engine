/*
	Event Handler
	
	Copyright 2006 Daniel G. Taylor
*/

#ifndef _EVENT_H
#define _EVENT_H

#include <queue>
#include <sigc++/sigc++.h>

#include "log.h"

using namespace std;

namespace Boom
{
	enum DefaultEvents
	{
		EVENT_QUIT,				// void
		EVENT_STATE_CHANGED,	// void
		EVENT_KEY_DOWN,			// int
		EVENT_KEY_UP,			// int
		EVENT_NULL				// placeholder, used internally
	};
	
	namespace Event
	{
		typedef unsigned int Event;
		
		enum Type
		{
			TYPE_VOID,			// No arguments
			TYPE_INT,			// Single int argument
			TYPE_FLOAT,			// Single float argument
			TYPE_STRING			// Single string argument
		};
		
		enum Priority
		{
			PRIORITY_LOW,		// Processed when there is extra time
			PRIORITY_NORMAL,	// Processed as soon as possible
			PRIORITY_HIGH		// Processed immediately
		};
		
		extern unsigned short process_max;	// Maximum number of events to process
											// in a single call to process()
											
		const unsigned short PROCESS_MAX_DEFAULT = 15;
		
		// Initialize and cleanup the event module
		void init();
		void cleanup();
		
		// Add a new unique event identifier
		void add(Type type, Event event);
		
		// Connect a function to an event identifier
		void connect(Event event, void (*func)(void));
		void connect(Event event, void (*func)(int));
		void connect(Event event, void (*func)(float));
		void connect(Event event, void (*func)(const char *));
		
		// Post an event to the queue
		void post(Event event, Priority priority = PRIORITY_NORMAL);
		void post(Event event, int arg, Priority priority = PRIORITY_NORMAL);
		void post(Event event, float arg, Priority priority = PRIORITY_NORMAL);
		void post(Event event, const char *arg, Priority priority = PRIORITY_NORMAL);
		
		// Process the events posted to the queue
		void process();
	}
}

#endif
