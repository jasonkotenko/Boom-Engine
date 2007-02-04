/*
	Camera classes and functions
*/

#include "camera.h"

namespace Boom
{
	namespace Scene
	{
		Camera::Camera(Point3d pos, Point3d lookat, Point3d up)
		{
			this->pos = pos;
			this->lookat = lookat;
			this->up = up;
		}
		
		void Camera::update()
		{
			
		}
		
		void Camera::render()
		{
			gluLookAt(pos.x,    pos.y,    pos.z,
					  lookat.x, lookat.y, lookat.z,
					  up.x,     up.y,     up.z);
		}
	}
}
