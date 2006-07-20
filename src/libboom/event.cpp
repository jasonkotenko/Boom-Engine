/*
	Event impelementation
	
	Copyright 2006 Daniel G. Taylor
*/

#include <map>
#include <queue>
#include <sigc++/sigc++.h>

#include "event.h"
#include "log.h"

using namespace std;

namespace Boom
{
	namespace Event
	{
		struct PostedEvent
		{
			Event		name;
			Priority	priority;
			
			virtual ~PostedEvent() {}
		};
		
		struct PostedEventInt: public PostedEvent
		{
			int arg;
		};
		
		struct PostedEventFloat: public PostedEvent
		{
			float arg;
		};
		
		struct PostedEventString: public PostedEvent
		{
			const char *arg;
		};
		
		unsigned short process_max = PROCESS_MAX_DEFAULT;
		
		map <Event, Type>								type_map;
		map <Event, sigc::signal <void> >				sig_void;
		map <Event, sigc::signal <void, int> >			sig_int;
		map <Event, sigc::signal <void, float> >		sig_float;
		map <Event, sigc::signal <void, const char *> >	sig_string;
		
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
	
		void init()
		{
			LOG_INFO << "Initializing event system..." << endl;
			add(TYPE_VOID, EVENT_QUIT);
			add(TYPE_VOID, EVENT_STATE_CHANGED);
			add(TYPE_INT,  EVENT_KEY_DOWN);
			add(TYPE_INT,  EVENT_KEY_UP);
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
		
		void add(Type type, Event event)
		{
			sigc::signal <void> *sv;
			sigc::signal <void, int> *si;
			sigc::signal <void, float> *sf;
			sigc::signal <void, const char *> *ss;
			
			switch(type)
			{
				case TYPE_VOID:
					sv = new sigc::signal <void> ();
					sig_void[event] = *sv;
					delete sv;
					break;
				case TYPE_INT:
					si = new sigc::signal <void, int> ();
					sig_int[event] = *si;
					delete si;
					break;
				case TYPE_FLOAT:
					sf = new sigc::signal <void, float> ();
					sig_float[event] = *sf;
					delete sf;
					break;
				case TYPE_STRING:
					ss = new sigc::signal <void, const char *> ();
					sig_string[event] = *ss;
					delete ss;
					break;
				default:
					LOG_WARNING << "Uknown type '" << type << "' passed to Event::add()"
								<< endl;
					return;
			}
			type_map[event] = type;
		}
		
		void connect(Event event, void (*func)(void))
		{
			sig_void[event].connect(sigc::ptr_fun(func));
		}
		
		void connect(Event event, void (*func)(int))
		{
			sig_int[event].connect(sigc::ptr_fun(func));
		}
		
		void connect(Event event, void (*func)(float))
		{
			sig_float[event].connect(sigc::ptr_fun(func));
		}
		
		void post(Event event, Priority priority)
		{
			PostedEvent *e = new PostedEvent();
			
			e->name = event;
			e->priority = priority;
			
			posted_events.push(e);
		}
		
		void post(Event event, int arg, Priority priority)
		{
			PostedEventInt *e = new PostedEventInt();
			
			e->name = event;
			e->arg = arg;
			e->priority = priority;
			
			posted_events.push(static_cast <PostedEvent*> (e));
		}
		
		void post(Event event, float arg, Priority priority)
		{
			PostedEventFloat *e = new PostedEventFloat();
			
			e->name = event;
			e->arg = arg;
			e->priority = priority;
			
			posted_events.push(static_cast <PostedEvent*> (e));
		}
		
		void post(Event event, const char *arg, Priority priority)
		{
			PostedEventString *e = new PostedEventString();
			
			e->name = event;
			e->arg = arg;
			e->priority = priority;
			
			posted_events.push(static_cast <PostedEvent*> (e));
		}
		
		void process()
		{
			PostedEvent *event;
			unsigned short processed = 0;
			
			while (posted_events.size() > 0 && processed < process_max)
			{
				event = posted_events.top();
				
				switch(type_map[event->name])
				{
					case TYPE_VOID:
						sig_void[event->name]();
						break;
					case TYPE_INT:
						sig_int[event->name](dynamic_cast <PostedEventInt*> (event)->arg);
						break;
					case TYPE_FLOAT:
						sig_float[event->name](dynamic_cast <PostedEventFloat*> (event)->arg);
						break;
					case TYPE_STRING:
						sig_string[event->name](dynamic_cast <PostedEventString*> (event)->arg);
						break;
					default:
						break;
				}
				
				delete event;
				posted_events.pop();
				processed++;
			}
		}
	}
}
