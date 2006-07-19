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
	enum EventType
	{
		EVENT_QUIT,				// void
		EVENT_STATE_CHANGED,	// void
		EVENT_KEY_DOWN,			// int
		EVENT_KEY_UP,			// int
		EVENT_NULL				// placeholder, used internally
	};
	
	enum PriorityType
	{
		PRIORITY_LOW,		// Processed when there is extra time
		PRIORITY_NORMAL,	// Processed as soon as possible
		PRIORITY_HIGH		// Processed immediately
	};
	
	extern unsigned short process_max;	// Maximum number of events to process
										// in a single call to process()
	const unsigned short PROCESS_MAX_DEFAULT = 15;
	
	namespace Event
	{	
		void init();
		void cleanup();
		
		void connect(EventType type, void (*func)(void));
		void connect(EventType type, void (*func)(int));
		void connect(EventType type, void (*func)(float));
		
		void post(EventType type, PriorityType priority = PRIORITY_NORMAL);
		void post(EventType type, int arg, PriorityType priority = PRIORITY_NORMAL);
		void post(EventType type, float arg, PriorityType priority = PRIORITY_NORMAL);
		
		void process();
	}
}

#endif _EVENT_H
