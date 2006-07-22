/*
	Interface classes
*/

#ifndef _INTERFACE_H
#define _INTERFACE_H

namespace Boom
{
	extern double tdiff;

	namespace Interface
	{
		double seconds();
		
		class BaseInterface
		{
			public:
				BaseInterface();
				virtual ~BaseInterface();
				
				virtual void start();
				
			protected:
				virtual void init_gl();
				virtual void draw();
				virtual void resize(int width, int height);
				virtual void queue_flip();
				virtual void calc_tdiff();
				
				double last_time;
		};
	}
}

#endif
