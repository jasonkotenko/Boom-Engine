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

from Graphics import Point3d, PolarVector3d
from math import sin, cos, pi, fabs
from copy import deepcopy
import Interface

# Ease of use constants
threepi = pi * 3.0
twopi = pi * 2.0
pi4 = pi / 4.0
pi2 = pi / 2.0
pi34 = 3.0 * pi / 4.0
pi54 = 5.0 * pi / 4.0
pi32 = 3.0 * pi / 2.0
pi74 = 7.0 * pi / 4.0
pi94 = 9.0 * pi / 4.0

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

#-------------------------------------------------------------------------------
class CubeCamera(Camera3d):
	# Face constants
	FACE1 = 0 # PolarVector3d(1.0, 0.0, 0.0)
	FACE2 = 1 # PolarVector3d(1.0, 3.0 * pi / 2.0, pi / 2.0)
	FACE3 = 2 # PolarVector3d(1.0, pi, pi / 2.0)
	FACE4 = 3 # PolarVector3d(1.0, pi / 2.0, pi / 2.0)
	FACE5 = 4 # PolarVector3d(1.0, 0.0, pi / 2.0)
	FACE6 = 5 # PolarVector3d(1.0, 0.0, pi)

	# Face (up/normal) Array
	# As of this point continue at your own risk
	FACE_ARRAY = [PolarVector3d(1.0, 0.0, 0.0), PolarVector3d(1.0, pi32, pi2), \
				PolarVector3d(1.0, pi, pi2), PolarVector3d(1.0, pi2, pi2), \
				PolarVector3d(1.0, 0.0, pi2), PolarVector3d(1.0, 0.0, pi), \
				PolarVector3d(1.0, pi2, 0.0), PolarVector3d(1.0, pi, 0.0), \
				PolarVector3d(1.0, pi32, 0.0), PolarVector3d(1.0, pi2, pi), \
				PolarVector3d(1.0, pi, pi), PolarVector3d(1.0, pi32, pi), \
				PolarVector3d(1.0, -pi2, pi2), PolarVector3d(1.0, twopi, pi2)]

	# Array for all possible positions. It should only be twelve elements
	# long, but the way I currently do the math, I need all possible
	# positions for all possible vectors to make the animation move along
	# the "correct" path. The last fourteen vectors are "ease of use" vectors
	# that make the math a lot simpler. I will try on moving the vectors
	# into calculations instead of wasting memory holding this many variables
	POSITION_ARRAY = [PolarVector3d(1.0, 0.0, pi4), PolarVector3d(1.0, pi2, pi4), \
					PolarVector3d(1.0, pi, pi4), PolarVector3d(1.0, pi32, pi4), \
					PolarVector3d(1.0, pi4, pi2), PolarVector3d(1.0, pi34, pi2), \
					PolarVector3d(1.0, pi54, pi2), PolarVector3d(1.0, pi74, pi2), \
					PolarVector3d(1.0, 0.0, pi34), PolarVector3d(1.0, pi2, pi34), \
					PolarVector3d(1.0, pi, pi34), PolarVector3d(1.0, pi32, pi34), \
					PolarVector3d(1.0, -pi4, pi2), PolarVector3d(1.0, pi2, -pi4), \
					PolarVector3d(1.0, 0.0, -pi4), PolarVector3d(1.0, pi32, -pi4), \
					PolarVector3d(1.0, pi, -pi4), PolarVector3d(1.0, twopi, pi4), \
					PolarVector3d(1.0, pi94, pi2), PolarVector3d(1.0, twopi, pi34), \
					PolarVector3d(1.0, pi32, pi54), PolarVector3d(1.0, pi, pi54), \
					PolarVector3d(1.0, pi2, pi54), PolarVector3d(1.0, -pi2, pi4), \
					PolarVector3d(1.0, -pi2, pi34), PolarVector3d(1.0, 0.0, pi54)]


	# Ludicrous lookup! Do not look any lower if you want to keep your sanity!
	# Format is: LOOK_UP[current_face][final_face] = [[current_pos, final_pos], 
	# [current_pos, final_pos], [current_pos, final_pos], [current_pos, final_pos]]
	# The positions are references to POSITION_ARRAY's index
	LOOK_UP = [[[None], \
			[[0, 12], [1, 13], [2, 6], [3, 11]], \
			[[0, 14], [1, 5], [2, 10], [3, 6]], \
			[[0, 4], [1, 9], [2, 5], [3, 15]], \
			[[0, 8], [1, 4], [2, 16], [3, 7]], \
			[None]], \
			[[[3, 15], [6, 2], [7, 17], [11, 3]], \
			[None], \
			[[3, 2], [6, 5], [7, 6], [11, 10]], \
			[None], \
			[[3, 17], [6, 7], [7, 18], [11, 19]], \
			[[3, 11], [6, 10], [7, 19], [11, 20]]], \
			[[[2, 16], [5, 1], [6, 3], [10, 2]], \
			[[2, 3], [5, 6], [6, 7], [10, 11]], \
			[None], \
			[[2, 1], [5, 4], [6, 5], [10, 9]], \
			[None], \
			[[2, 10], [5, 9], [6, 11], [10, 21]]], \
			[[[1, 13], [4, 0], [5, 2], [9, 1]], \
			[None], \
			[[1, 2], [4, 5], [5, 6], [9, 10]], \
			[None], \
			[[1, 0], [4, 12], [5, 4], [9, 8]], \
			[[1, 9], [4, 8], [5, 10], [9, 22]]], \
			[[[0, 14], [4, 1], [7, 3], [8, 0]], \
			[[0, 23], [4, 12], [7, 6], [8, 24]], \
			[None], \
			[[0, 1], [4, 5], [7, 18], [8, 9]], \
			[None], \
			[[0, 8], [4, 9], [7, 11], [8, 25]]], \
			[[None], \
			[[8, 12], [9, 22], [10, 6], [11, 3]], \
			[[8, 25], [9, 5], [10, 2], [11, 6]], \
			[[8, 4], [9, 1], [10, 5], [11, 22]], \
			[[8, 0], [9, 4], [10, 21], [11, 7]], \
			[None]], \
			]


	def __init__(self, pos = 0, zoom = 30.0, cube_size = 1.0, up = 0):
		self.up_vectors = [up, up]
		self.pos_vectors = [pos, pos]
		self.zooms = [zoom, zoom]
		# Cube_size defines the size of the underlying cube
		# Divided in half for calculations for gluLookAt
		self.cube_size = cube_size / 2.0
		# Make a copy so we don't change the references
		self.posp = deepcopy(self.POSITION_ARRAY[pos])
		self.upp = deepcopy(self.FACE_ARRAY[up])
		# Set the current distance to the zoom we are passed
		self.posp.rho = zoom
		# Initialize Point3d variables to represent position, lookat, and up
		Camera3d.__init__(self, self.posp.get_xyz(), self.get_lookat(), self.upp.get_xyz())
		# Animation time values, both [current time, total]
		self.rotate_time = [0.0, 0.0]
		self.zoom_time = [0.0, 0.0]
		# Animation acceleration, [Position, Up] <Deprecated>
		self.acceleration = [PolarVector3d(0.0, 0.0, 0.0), PolarVector3d(0.0, 0.0, 0.0)]
		self.animated = [False, False] # [rotate, zoom]

	def get_lookat(self):
		if self.cube_size > 0:
			self.upp.rho *= self.cube_size
			retval = self.upp.get_xyz()
			self.upp.rho /= self.cube_size
		else:
			retval = Point3d(0.0, 0.0, 0.0)
		return retval

	def zoom(self, time, new_rho):
		if self.animated[1] or self.posp.rho == new_rho:
			return
		self.animated[1] = True
		self.zooms = [self.posp.rho, new_rho]
		self.zoom_time = [0.0, time]

	def rotate(self, time, new_up, new_pos = -1):
		if self.animated[0]:
			return
		self.animated[0] = True
		self.rotate_time = [0.0, time]
		curr_up = self.up_vectors[1]
		if new_pos == -1:
			if curr_up == 13:
				curr_up = 4
			elif curr_up == 12:
				curr_up = 1
			elif curr_up > 8:
				curr_up = 5
			elif curr_up > 5:
				curr_up = 0
			tmp = LOOK_UP[curr_up][new_up]
			for pos in tmp:
				if pos == None:
					self.animated[0] = False
					return
				if self.posp.theta == self.POSITION_ARRAY[pos[0]].theta and self.posp.phi == self.POSITION_ARRAY[pos[0]].phi:
					new_pos = pos[1]
					break

		if curr_up == 0:
			if new_up == 1:
				curr_up = 8
			elif new_up == 2:
				curr_up = 7
			elif new_up == 3:
				curr_up = 6
		elif curr_up == 5:
			if new_up == 1:
				curr_up = 11
			elif new_up == 2:
				curr_up = 10
			elif new_up == 3:
				curr_up = 9
		elif new_up == 0:
			if curr_up == 1:
				new_up = 8
			elif curr_up == 2:
				new_up = 7
			elif curr_up == 3:
				new_up = 6
		elif new_up == 5:
			if curr_up == 1:
				new_up = 11
			elif curr_up == 2:
				new_up = 10
			elif curr_up == 3:
				new_up = 9
		elif curr_up == 4 and new_up == 1:
			new_up = 12
		elif curr_up == 1 and new_up == 4:
			new_up = 13

		self.pos_vectors = [self.pos_vectors[1], new_pos]
		self.up_vectors = [curr_up, new_up]

		self.acceleration[0].theta = self.POSITION_ARRAY[self.pos_vectors[1]].theta - self.posp.theta
		self.acceleration[0].phi = self.POSITION_ARRAY[self.pos_vectors[1]].phi - self.posp.phi

		self.acceleration[1].theta = self.FACE_ARRAY[self.up_vectors[1]].theta - self.upp.theta
		self.acceleration[1].phi = self.FACE_ARRAY[self.up_vectors[1]].phi - self.upp.phi

	def update(self):
		if self.animated[0]:
			self.rotate_step()
			self.posp.rho = self.zooms[1]
		if self.animated[1]:
			self.zoom_step()
		if True in self.animated:
			self.pos = self.posp.get_xyz()
			self.up = self.upp.get_xyz()
			self.lookat = self.get_lookat()

	def rotate_step(self):
		self.rotate_time[0] += Interface.tdiff
		if self.rotate_time[0] <= self.rotate_time[1]:
			tmpvar = 3.0 * pi * self.rotate_time[0] / self.rotate_time[1]
			sintmpvar = sin(tmpvar)
			percent = 1 - (sintmpvar / tmpvar) + .1 * (sintmpvar)

			self.upp.theta = (self.FACE_ARRAY[self.up_vectors[1]].theta - self.FACE_ARRAY[self.up_vectors[0]].theta) * percent + self.FACE_ARRAY[self.up_vectors[0]].theta
			self.upp.phi = (self.FACE_ARRAY[self.up_vectors[1]].phi - self.FACE_ARRAY[self.up_vectors[0]].phi) * percent + self.FACE_ARRAY[self.up_vectors[0]].phi

			if self.acceleration[0].theta != 0.0 and self.acceleration[0].phi != 0.0:
				if self.POSITION_ARRAY[self.pos_vectors[0]].phi != self.FACE_ARRAY[self.up_vectors[0]].phi:
					self.posp.theta = sin(fabs(self.upp.phi - self.FACE_ARRAY[self.up_vectors[0]].phi)) * (self.POSITION_ARRAY[self.pos_vectors[1]].theta - self.POSITION_ARRAY[self.pos_vectors[0]].theta) + self.POSITION_ARRAY[self.pos_vectors[0]].theta
					self.posp.phi = (1 - cos(fabs(self.upp.phi - self.FACE_ARRAY[self.up_vectors[0]].phi))) * (self.POSITION_ARRAY[self.pos_vectors[1]].phi - self.POSITION_ARRAY[self.pos_vectors[0]].phi) + self.POSITION_ARRAY[self.pos_vectors[0]].phi
				else:
					self.posp.theta = (1 - cos(fabs(self.upp.phi - self.FACE_ARRAY[self.up_vectors[0]].phi))) * (self.POSITION_ARRAY[self.pos_vectors[1]].theta - self.POSITION_ARRAY[self.pos_vectors[0]].theta) + self.POSITION_ARRAY[self.pos_vectors[0]].theta
					self.posp.phi = sin(fabs(self.upp.phi - self.FACE_ARRAY[self.up_vectors[0]].phi)) * (self.POSITION_ARRAY[self.pos_vectors[1]].phi - self.POSITION_ARRAY[self.pos_vectors[0]].phi) + self.POSITION_ARRAY[self.pos_vectors[0]].phi

			elif self.acceleration[0].theta != 0.0:
				self.posp.theta = percent * (self.POSITION_ARRAY[self.pos_vectors[1]].theta - self.POSITION_ARRAY[self.pos_vectors[0]].theta) + self.POSITION_ARRAY[self.pos_vectors[0]].theta
			else:
				self.posp.phi = percent * (self.POSITION_ARRAY[self.pos_vectors[1]].phi - self.POSITION_ARRAY[self.pos_vectors[0]].phi) + self.POSITION_ARRAY[self.pos_vectors[0]].phi

			self.posp.theta %= 2.0 * pi
			self.posp.phi %= 2.0 * pi
			self.upp.theta %= 2.0 * pi
			self.upp.phi %= 2.0 * pi
		else:
			self.posp = deepcopy(self.POSITION_ARRAY[self.pos_vectors[1]])
			self.upp = deepcopy(self.FACE_ARRAY[self.up_vectors[1]])
			self.upp.phi %= twopi
			self.posp.phi %= twopi
			if self.upp.phi > pi:
				self.upp.theta += pi
				self.upp.phi = pi - (self.upp.phi % pi)
			if self.posp.phi > pi:
				self.posp.theta += pi
				self.posp.phi = pi - (self.posp.phi % pi)
			self.posp.theta %= twopi
			self.upp.theta %= twopi

			# Reverse lookup
			for p in range(len(self.POSITION_ARRAY)):
				if self.posp.equals(self.POSITION_ARRAY[p]):
					self.pos_vectors[1] = p
					break

			self.rotate_time = [0.0, 0.0]
			for i in self.acceleration:
				i.theta = 0.0
				i.phi = 0.0
			self.animated[0] = False

	def zoom_step(self):
		#global tdiff
		self.zoom_time[0] += Interface.tdiff
		if self.zoom_time[0] <= self.zoom_time[1]:
			tmpvar = 3.0 * pi * self.zoom_time[0] / self.zoom_time[1]
			sintmpvar = sin(tmpvar)
			percent = 1 - (sintmpvar / tmpvar) + .1 * (sintmpvar)
			self.posp.rho = percent * (self.zooms[1] - self.zooms[0]) + self.zooms[0]
		else:
			self.posp.rho = self.zooms[1]
			self.zoom_time = [0.0, 0.0]
			self.acceleration[0].rho = 0.0
			self.animated[1] = False