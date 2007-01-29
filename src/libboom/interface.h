/*
	Interface classes
*/

#ifndef _INTERFACE_H
#define _INTERFACE_H

namespace Boom
{
	//! The time difference since the last frame
	extern double tdiff;

	/// The interface namespace
	/*!
		This contains functions and objects to setup an interface through which
		the user can interact and be displayed data, as well as the game's main
		loop.
	*/
	namespace Interface
	{
		//! Calculate the seconds since the epoch with high precision
		double seconds();
		
		/// A base interface class
		/*!
			This is to be inherited by actual interfaces. The inherited class
			should provide a start function to actually e.g. create a window
			and start the main loop drawing (using e.g. SDL or GLUT).
		*/
		class BaseInterface
		{
			public:
				BaseInterface();
				virtual ~BaseInterface();
				
				//! Start the main loop
				virtual void start();
				
			protected:
				//! Initialize OpenGL
				virtual void init_gl();
				//! Draw the current state (menu, scene, etc)
				virtual void draw();
				//! Reset the projection when a window is resized
				virtual void resize(int width, int height);
				//! Queue a flip of the screen buffer
				virtual void queue_flip();
				//! Calculate the time difference since the last frame
				virtual void calc_tdiff();
				
				//! Stores the result of a call to seconds to calculate time diffs
				double last_time;
		};
	}
}

#endif
