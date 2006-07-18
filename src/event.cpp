/*
	Event impelementation
	
	Copyright 2006 Daniel G. Taylor
*/

#include "event.h"

namespace Boom
{
	namespace Event
	{
		struct PostedEvent
		{
			EventType		type;
			PriorityType	priority;
		};
		
		struct PostedEventInt: public PostedEvent
		{
			int arg;
		};
		
		struct PostedEventFloat: public PostedEvent
		{
			float arg;
		};
		
		struct PostedEventComparison
		{
			bool operator() (const PostedEvent *left, const PostedEvent *right)
			{
				return left->priority < right->priority;
			}
		};
		
		unsigned short process_max = 15;
		
		sigc::signal<void> sig_quit;
		sigc::signal<void> sig_state_changed;
		
		sigc::signal<void, int> sig_key_down;
		sigc::signal<void, int> sig_key_up;
		
		priority_queue <PostedEvent*, deque <PostedEvent*>, PostedEventComparison> posted_events;
	
		void init()
		{
			LOG_INFO << "Initializing event system..." << endl;
		}
		
		void cleanup()
		{
			PostedEvent *event;
			while (posted_events.size() > 0)
			{
				event = posted_events.top();
				delete event;
				posted_events.pop();
			}
		}
		
		void connect(EventType type, void (*func)(void))
		{
			switch(type)
			{
				case EVENT_QUIT:
					sig_quit.connect(sigc::ptr_fun(func));
					break;
				case EVENT_STATE_CHANGED:
					sig_state_changed.connect(sigc::ptr_fun(func));
					break;
			}
		}
		
		void connect(EventType type, void (*func)(int))
		{
			switch(type)
			{
				case EVENT_KEY_DOWN:
					sig_key_down.connect(sigc::ptr_fun(func));
					break;
				case EVENT_KEY_UP:
					sig_key_up.connect(sigc::ptr_fun(func));
					break;
			}
		}
		
		void connect(EventType type, void (*func)(float))
		{
			switch(type)
			{
				
			}
		}
		
		void post(EventType type, PriorityType priority)
		{
			PostedEvent *event = new PostedEvent();
			
			event->type = type;
			event->priority = priority;
			
			posted_events.push(event);
		}
		
		void post(EventType type, int arg, PriorityType priority)
		{
			PostedEventInt *event = new PostedEventInt();
			
			event->type = type;
			event->arg = arg;
			event->priority = priority;
			
			posted_events.push(static_cast<PostedEvent*>(event));
		}
		
		void post(EventType type, float arg, PriorityType priority)
		{
			PostedEventFloat *event = new PostedEventFloat();
			
			event->type = type;
			event->arg = arg;
			event->priority = priority;
			
			posted_events.push(static_cast<PostedEvent*>(event));
		}
		
		void process()
		{
			PostedEvent *event;
			unsigned short processed = 0;
			
			while (posted_events.size() > 0 && processed < process_max)
			{
				event = posted_events.top();
				
				switch(event->type)
				{
					case EVENT_QUIT:
						sig_quit();
						delete event;
						break;
					case EVENT_STATE_CHANGED:
						sig_state_changed();
						delete event;
						break;
					case EVENT_KEY_DOWN:
						sig_key_down(reinterpret_cast<PostedEventInt*>(event)->arg);
						delete reinterpret_cast<PostedEventInt*>(event);
						break;
					case EVENT_KEY_UP:
						sig_key_up(reinterpret_cast<PostedEventInt*>(event)->arg);
						delete reinterpret_cast<PostedEventInt*>(event);
						break;
				}
				
				posted_events.pop();
				processed++;
			}
		}
	}
}