/*
	Inteface classes implementation
*/

#include <sys/time.h>

#include "gl.h"
#include "interface.h"
#include "state.h"
#include "log.h"

using namespace Boom;

namespace Boom
{
	double tdiff;
	
	namespace Interface
	{
		double seconds()
		{
			timeval t;
			double s;
			
			gettimeofday(&t, NULL);
			s = t.tv_sec + (t.tv_usec / 1000000.0);
			
			return s;
		}
		
		BaseInterface::BaseInterface()
		{
			tdiff = 0;
			last_time = seconds();
		}
		
		BaseInterface::~BaseInterface()
		{
		
		}
		
		void BaseInterface::start()
		{
		
		}
		
		void BaseInterface::init_gl()
		{
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClearDepth(1.0);
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			
			glEnable(GL_LIGHT0);
			float position[] = {50.0, 50.0, 150.0, 0.0};
			float ambient[] = {0.2, 0.2, 0.2};
			float diffuse[] = {1.0, 1.0, 1.0};
			float specular[] = {1.0, 1.0, 1.0};
			glLightfv(GL_LIGHT0, GL_POSITION, position);
			glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
			glEnable(GL_LIGHTING);
			
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_TEXTURE_2D);
		}
		
		void BaseInterface::draw()
		{
			// Update the time since last frame
			calc_tdiff();
		
			// Clear the frame and draw the current state
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
			
			State::draw();
			
			// Update the display
			queue_flip();
		}
		
		void BaseInterface::queue_flip()
		{
			glFlush();
		}
		
		void BaseInterface::resize(int width, int height)
		{
			// Reset the camer/view to the new width/height
			glViewport(0, 0, width, height);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(25.0, float(width)/float(height), 10.0, 100.0);
			glMatrixMode(GL_MODELVIEW);
		}
		
		void BaseInterface::calc_tdiff()
		{
			double current = seconds();
			tdiff = current - last_time;
			//LOG_INFO << tdiff << ", " << current << ", " << last_time << endl;
			last_time = current;
		}
		
		KeyboardMovement::KeyboardMovement()
		{
			angle = 0.0;
			_left = _right = _up = _down = moving = false;
		}
		
		void KeyboardMovement::right()
		{
			_right = !_right;
			update_angle();
			update_moving();
		}
		
		void KeyboardMovement::left()
		{
			_left = !_left;
			update_angle();
			update_moving();
		}
		
		void KeyboardMovement::up()
		{
			_up = !_up;
			update_angle();
			update_moving();
		}
		
		void KeyboardMovement::down()
		{
			_down = !_down;
			update_angle();
			update_moving();
		}
		
		void KeyboardMovement::update_angle()
		{
			if (_up && _left)
				angle = 2.3562;
			else if (_up && _right)
				angle = 0.7854;
			else if (_down && _left)
				angle = 3.9270;
			else if (_down && _right)
				angle = 5.4978;
			else if (_up)
				angle = 1.5708;
			else if (_down)
				angle = 4.7124;
			else if (_left)
				angle = 3.1416;
			else if (_right)
				angle = 0.0;
		}
		
		void KeyboardMovement::update_moving()
		{
			moving = _up || _down || _left || _right;
		}
	}
}
