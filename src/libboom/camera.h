/*
	Camera classes and functions
*/

#ifndef _CAMERA_H
#define _CAMERA_H

#include "graphics.h"

namespace Boom
{
	using namespace Graphics;
	
	namespace Scene
	{
		struct Camera
		{
			Point3d pos;
			Point3d lookat;
			Point3d up;
			
			Camera(Point3d pos    = Point3d(0, -25, 25),
				   Point3d lookat = Point3d(0, 0, 0),
				   Point3d up     = Point3d(0, 0, 1));
			virtual void update();
			virtual void render();
		};
	}
}

#endif
