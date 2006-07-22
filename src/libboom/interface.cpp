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
			s = t.tv_sec + (t.tv_usec / 10000000.0);
			
			return s;
		}
		
		BaseInterface::BaseInterface()
		{
			tdiff = 0;
			last_time = 0;
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
			last_time = current;
		}
	}
}
