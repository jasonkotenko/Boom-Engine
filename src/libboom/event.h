/*
	Event Handler
	
	Copyright 2006 Daniel G. Taylor
*/

#ifndef _EVENT_H
#define _EVENT_H

#include <map>
#include <sigc++/sigc++.h>

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
		typedef unsigned int EventID;
		
		enum Priority
		{
			PRIORITY_LOW,		// Processed when there is extra time
			PRIORITY_NORMAL,	// Processed as soon as possible
			PRIORITY_HIGH		// Processed immediately
		};
		
		struct EventData
		{
			sigc::signal <void, void *> signal;
			sigc::slot <void, void *> deallocator;
		};
		
		extern unsigned short process_max;	// Maximum number of events to process
											// in a single call to process()
											
		const unsigned short PROCESS_MAX_DEFAULT = 15;
		
		//extern map <EventID, sigc::signal <void, void *> > signals;
		extern map <EventID, EventData> signals;
		
		// Initialize and cleanup the event module
		void init();
		void cleanup();
		
		// Add a new unique event identifier
		void add(EventID event, void (*deallocator)(void *) = NULL);
		
		template <class Object>
		void add(EventID event, Object *instance, void (Object::*deallocator)(void *))
		{
			EventData data;
			
			data.deallocator = sigc::mem_fun(*instance, deallocator);
			
			signals[event] = data;
		}
		
		// Connect a function to an event identifier
		void connect(EventID event, void (*func)(void *));
		
		template <class Object>
		void connect(EventID event, Object *instance, void (Object::*func)(void *))
		{
			signals[event].signal.connect(sigc::mem_fun(*instance, func));
		}
		
		// Post an event to the queue
		void post(EventID event, void *args = NULL, Priority priority = PRIORITY_NORMAL);
		
		// Process posted events
		void process();
	}
}

#endif
