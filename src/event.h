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
		EVENT_QUIT,
		EVENT_STATE_CHANGED,
		EVENT_KEY_DOWN,
		EVENT_KEY_UP,
		EVENT_NULL
	};
	
	enum PriorityType
	{
		PRIORITY_LOW,		// Processed when there is extra time
		PRIORITY_NORMAL,	// Processed as soon as possible
		PRIORITY_HIGH		// Processed immediately
	};
	
	extern unsigned short process_max;	// Maximum number of events to process
										// in a single call to process()
	
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
