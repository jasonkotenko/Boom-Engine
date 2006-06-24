#!/usr/bin/env python

"""
	Boom Game Objects
	=================
		Classes to define game entities such as a level, player, item, bomb, etc. This
		is the meat of the actual game.
	
		License
		-------
		Copyright (C) 2006 Daniel G. Taylor

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

import DataManager
import Interface
import Log
import Event

from Graphics import *

from math import sin, sqrt, pow, degrees, pi

ITEM_ANIM_NONE = 0
ITEM_ANIM_THROB = 1
ITEM_ANIM_ROTATE = 2
ITEM_ANIM_BOUNCE = 3

#-------------------------------------------------------------------------------
class Level:
	def __init__(self, name = None):
		self.mesh = name + ".obj"
		self.players = []
		self.player = None
		self.items = []
		self.timer = 180
	
	def add_player(self, name, x = 0, y = 0, control = False):
		player = Player()
		player.name = name
		player.x = x
		player.y = y
		self.players.append(player)
		if control:
			self.player = player
	
	def add_bomb(self, x = 0, y = 0):
		bomb = Bomb()
		bomb.x = x
		bomb.y = y
		bomb.timer = 3
		self.items.append(bomb)
	
	def update(self):
		for pos in range(len(self.players) - 1, -1, -1):
			retval = self.players[pos].update(self)
			if retval == False:
				del self.players[pos]
		for pos in range(len(self.items) - 1, -1, -1):
			retval = self.items[pos].update(self)
			if retval == False:
				del self.items[pos]
	
	def draw(self):
		DataManager.meshes[self.mesh].render()
		for player in self.players:
			player.draw()
		for item in self.items:
			item.draw()

#-------------------------------------------------------------------------------
class Movement:
	def __init__(self):
		self.up = False
		self.down = False
		self.left = False
		self.right = False

#-------------------------------------------------------------------------------
class GameObject:
	def __init__(self, mesh = None, x = 0, y = 0):
		self.id = 0
		self.mesh = mesh
		self.x = x
		self.y = y
		self.scale = 1.0
	
	def update(self, level):
		return True
	
	def draw(self):
		pass
	
	def draw(self):
		pass

#-------------------------------------------------------------------------------
class Player(GameObject):
	def __init__(self):
		GameObject.__init__(self)
		self.name = "Player"
		self.itemids = []
		self.moving = Movement()
		self.speed = 3
		self.life = 1
		self.mesh = "player.obj"
		self.current_angle = 0
	
	def update(self, level):
		newpos = Point2d(self.x, self.y)
		if self.moving.up:
			newpos.y = self.y + Interface.tdiff * self.speed
		elif self.moving.down:
			newpos.y = self.y - Interface.tdiff * self.speed
		if self.moving.left:
			newpos.x = self.x - Interface.tdiff * self.speed
		elif self.moving.right:
			newpos.x = self.x + Interface.tdiff * self.speed
		# Check if we are allowed to move there!
		allow_x = True
		allow_y = True
		mesh1 = DataManager.meshes[self.mesh]
		for player in level.players:
			if player != self:
				mesh2 = DataManager.meshes[player.mesh]
				xdiff = (newpos.x + mesh1.center.x) - (player.x + mesh2.center.x)
				ydiff = (newpos.y + mesh1.center.y) - (player.y + mesh2.center.y)
				length = sqrt(xdiff * xdiff + ydiff * ydiff)
				if length < (mesh1.radius + mesh2.radius) - 0.5:
					# Handle collision!
					allow_x = False
					allow_y = False
		
		for item in level.items:
			mesh2 = DataManager.meshes[item.mesh]
			length = sqrt(pow((newpos.x + mesh1.center.x) - (item.x + mesh2.center.x), 2) + pow((newpos.y + mesh1.center.y) - (item.y + mesh2.center.y), 2))
			length2 = sqrt(pow((self.x + mesh1.center.x) - (item.x + mesh2.center.x), 2) + pow((self.y + mesh1.center.y) - (item.y + mesh2.center.y), 2))
			if length < length2 - 0.05:
				if length < (mesh1.radius + mesh2.radius):
					allow_x = False
					allow_y = False
		
		mesh = DataManager.meshes[level.mesh]
		for poly in mesh.polygons:
			center = Point3d()
			v1 = mesh.vertices[poly.vertices[0][0]].pos
			v2 = mesh.vertices[poly.vertices[1][0]].pos
			v3 = mesh.vertices[poly.vertices[2][0]].pos
			center.x = (v1.x + v2.x + v3.x) / 3.0
			center.y = (v1.y + v2.y + v3.y) / 3.0
			center.z = (v1.z + v2.z + v3.z) / 3.0
			xdiff = newpos.x - center.x
			ydiff = newpos.y - center.y
			#print xdiff, ydiff
			length = sqrt(pow(xdiff, 2) + pow(ydiff, 2))
			#if length < radius:
				#allow_x = False
				#allow_y = False
		
		if allow_x:
			self.x = newpos.x
		if allow_y:
			self.y = newpos.y
		
		return True
	
	def draw(self):
		glPushMatrix()
		glTranslatef(self.x, self.y, 0.0)
		angle = self.current_angle
		if self.moving.up and self.moving.left:
			angle = 225
		elif self.moving.up and self.moving.right:
			angle = 135
		elif self.moving.down and self.moving.left:
			angle = 315
		elif self.moving.down and self.moving.right:
			angle = 45
		elif self.moving.up:
			angle = 180
		elif self.moving.down:
			angle = 0
		elif self.moving.left:
			angle = 270
		elif self.moving.right:
			angle = 90
		self.current_angle = angle
		glRotatef(self.current_angle, 0, 0, 1)
		DataManager.meshes[self.mesh].render()
		glPopMatrix()

#-------------------------------------------------------------------------------
class Item(GameObject):
	def __init__(self):
		GameObject.__init__(self)
		self.timer = 0
		self.type = "None"
		self.anim_angle = 0
		self.anim_speed = 5
		self.anim_type = ITEM_ANIM_NONE
	
	def update(self, level):
		retval = True
		self.timer -= Interface.tdiff
		if self.timer <= 0:
			retval = self.timeout()
		return retval
	
	def draw(self):
		self.anim_angle += Interface.tdiff * self.anim_speed
		if self.anim_angle >= 6.28:
			self.anim_angle = 0
		glPushMatrix()
		glTranslatef(self.x, self.y, 0.0)
		if self.anim_type == ITEM_ANIM_THROB:
			scale_factor = 1.0 + (sin(self.anim_angle) * 0.06)
			glScalef(scale_factor, scale_factor, scale_factor)
		elif self.anim_type == ITEM_ANIM_ROTATE:
			glRotatef(degrees(self.anim_angle), 0, 0, 1)
		elif self.anim_type == ITEM_ANIM_BOUNCE:
			if self.anim_angle < pi:
				height_diff = (self.anim_angle / pi) - 0.5
			else:
				height_diff = -(((self.anim_angle - pi) / pi) - 0.5)
			glTranslatef(0.0, 0.0, height_diff)
		DataManager.meshes[self.mesh].render()
		glPopMatrix()
	
	def timeout(self):
		return False

#-------------------------------------------------------------------------------
class Bomb(Item):
	def __init__(self):
		Item.__init__(self)
		self.type = "Bomb"
		self.mesh = "bomb.obj"
		self.anim_type = ITEM_ANIM_THROB
		self.radius = 2.0
		self.current_size = 0.5
		self.exploding = 0
	
	def update(self, level):
		if not self.exploding:
			return Item.update(self, level)
		else:
			size_diff = (self.radius - self.current_size)
			self.current_size += Interface.tdiff * (size_diff + 0.5) * 2
			if self.current_size >= self.radius:
				return False
				
			for pos in range(len(level.players) - 1, -1, -1):
				player = level.players[pos]
				xdiff = player.x - self.x
				ydiff = player.y - self.y
				if sqrt(xdiff * xdiff + ydiff * ydiff) <= self.current_size:
					player.life -= 1
					if player.life == 0:
						Log.info("Killed " + player.name)
						del level.players[pos]
						if len(level.players) == 1:
							Log.info(level.players[0].name + " wins the match!")
							Event.post(Event.QUIT)
					else:
						Log.info("Damaged " + player.name)
			
			for pos in range(len(level.items) - 1, -1, -1):
				item = level.items[pos]
				xdiff = item.x - self.x
				ydiff = item.y - self.y
				if sqrt(xdiff * xdiff + ydiff * ydiff) <= self.current_size:
					if item.type == "Bomb":
						item.timeout()
					else:
						Log.info("Destroyed " + item.type)
						del level.items[pos]
			return True
	
	def draw(self):
		if not self.exploding:
			Item.draw(self)
		else:
			percent = self.current_size / self.radius
			glPushMatrix()
			glTranslatef(self.x, self.y, 0.5)
			glEnable(GL_BLEND)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE)
			glDisable(GL_DEPTH_TEST)
			if percent <= 0.8:
				glMaterialfv(GL_FRONT, GL_DIFFUSE, [0.7, 0.7, 1.0, 0.5 * (0.8 - percent)])
				glutSolidSphere(pow(self.current_size, 3), 16, 16)
			glMaterialfv(GL_FRONT, GL_DIFFUSE, [0.8, 0.8 * percent, 0.8 * percent, 1.0 - percent])
			glutSolidSphere(self.current_size, 16, 16)
			glMaterialfv(GL_FRONT, GL_DIFFUSE, [1.0, 1.0, 0.8 * (1.0 - percent), 1.0 - percent])
			glutSolidSphere(0.5, 12, 12)
			glEnable(GL_DEPTH_TEST)
			glDisable(GL_BLEND)
			glPopMatrix()
	
	def timeout(self):
		self.exploding = True
		return True
