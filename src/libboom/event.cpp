/*
	Event impelementation
	
	Copyright 2006 Daniel G. Taylor
*/

#include <map>
#include <queue>

#include "event.h"
#include "log.h"

using namespace std;

namespace Boom
{
	namespace Event
	{
		struct PostedEvent
		{
			EventID		name;
			void		*args;
			Priority	priority;
		};
		
		unsigned short process_max = PROCESS_MAX_DEFAULT;
		
		map <EventID, EventData> signals;
		
		struct PostedEventComparison
		{
			bool operator() (const PostedEvent *left, const PostedEvent *right)
			{
				return left->priority < right->priority;
			}
		};
		
		priority_queue <PostedEvent*,
						deque <PostedEvent*>,
						PostedEventComparison> posted_events;
		
		void dealloc_int(void *args)
		{
			int *i = reinterpret_cast<int *>(args);
			
			delete i;
		}
	
		void init()
		{
			LOG_INFO << "Initializing event system..." << endl;
			add(EVENT_QUIT);
			add(EVENT_STATE_CHANGED);
			add(EVENT_KEY_DOWN, dealloc_int);
			add(EVENT_KEY_UP, dealloc_int);
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
		
		void add(EventID event, void (*deallocator)(void *))
		{
			EventData data;
			
			if (deallocator != NULL)
			{
				data.deallocator = sigc::ptr_fun(deallocator);
			}
			
			signals[event] = data;
		}
		
		void connect(EventID event, void (*func)(void *))
		{
			signals[event].signal.connect(sigc::ptr_fun(func));
		}
		
		void post(EventID event, void *args, Priority priority)
		{
			PostedEvent *e;
			
			if (!signals[event].signal.empty())
			{
				e = new PostedEvent();
			
				e->name = event;
				e->args = args;
				e->priority = priority;
			
				posted_events.push(e);
			}
		}
		
		void process()
		{
			PostedEvent *event;
			EventData *data;
			unsigned short processed = 0;
			
			while (posted_events.size() > 0 && processed < process_max)
			{
				event = posted_events.top();
				data = &signals[event->name];
				
				data->signal(event->args);
			
				if (data->deallocator)
				{
					data->deallocator(event->args);
				}
				
				delete event;
				posted_events.pop();
				processed++;
			}
		}
	}
}
