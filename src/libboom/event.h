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
	/// Default internal events
	/*!
		Events automatically registered for use within the engine and games.
	*/
	enum DefaultEvents
	{
		EVENT_QUIT,				// void
		EVENT_STATE_CHANGED,	// void
		EVENT_KEY_DOWN,			// int
		EVENT_KEY_UP,			// int
		EVENT_NULL				// placeholder, used internally
	};
	
	/// The event system namespace
	/*!
		Utilities to add events to the system, connect to and post events, and
		process the current event queue.
	*/
	namespace Event
	{
		typedef unsigned int EventID;
		
		/// Event priority
		/*!
			The priority of an event, from low to high. Higher priority events
			take precedence over lower priority ones.
		*/
		enum Priority
		{
			PRIORITY_LOW,		// Processed when there is extra time
			PRIORITY_NORMAL,	// Processed as soon as possible
			PRIORITY_HIGH		// Processed immediately
		};
		
		/// Information about an event
		/*!
			The signal and deallocator for an event type.
		*/
		struct EventData
		{
			sigc::signal <void, void *> signal;
			sigc::slot <void, void *> deallocator;
		};
		
		extern unsigned short process_max;	// Maximum number of events to process
											// in a single call to process()
											
		const unsigned short PROCESS_MAX_DEFAULT = 15;
		
		extern map <EventID, EventData> signals;
		
		//! Initialize the event module
		void init();
		
		//! Cleanup the event module
		void cleanup();
		
		//! Add a new unique event identifier (function)
		void add(EventID event, void (*deallocator)(void *) = NULL);
		
		//! Add a new unique event identifier (method)
		template <class Object>
		void add(EventID event, Object *instance, void (Object::*deallocator)(void *))
		{
			EventData data;
			
			data.deallocator = sigc::mem_fun(*instance, deallocator);
			
			signals[event] = data;
		}
		
		//! Connect a function to an event identifier
		void connect(EventID event, void (*func)(void *));
		
		//! Connect a method to an event identifier
		template <class Object>
		void connect(EventID event, Object *instance, void (Object::*func)(void *))
		{
			signals[event].signal.connect(sigc::mem_fun(*instance, func));
		}
		
		//! Post an event to the queue
		void post(EventID event, void *args = NULL, Priority priority = PRIORITY_NORMAL);
		
		//! Process posted events
		void process();
	}
}

#endif
