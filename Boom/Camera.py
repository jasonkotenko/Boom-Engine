#!/usr/bin/env python

#!/usr/bin/env python

"""
	Boom Camera
	===========
		Camera objects to store information about camera position and orientation,
		as well as control camera movement and rotation.
	
		License
		-------
		Copyright (C) 2006 Daniel G. Taylor, Jens Taylor

		This program is free software; you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation; either version 2 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with this program; if not, write to the Free Software
		Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
"""

from Graphics import Point3d

#-------------------------------------------------------------------------------
class Camera3d:
	"""
	Base Camera
	===========
		A simple camera object. Stores the position of the camera, the point that
		the camera is looking at, and an up vector.
		
		Inherit this for more complex camera systems.
	"""
	def __init__(self, pos = Point3d(0, -25, 25), lookat = Point3d(0, 0, 0), up = Point3d(0, 0, 1)):
		self.pos = pos
		self.lookat = lookat
		self.up = up
	
	def update(self):
		pass
