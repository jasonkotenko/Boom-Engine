#!/usr/bin/env python

"""
	Boom Game Objects
	=================
		Classes to define game entities such as a level, player, item, bomb, etc. This
		is the meat of the actual game.
	
		License
		-------
		Copyright (C) 2006 Daniel G. Taylor, Jason F. Kotenko

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

from math import sin, asin, sqrt, degrees, pi

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
		if control:
			player = Player()
			self.player = player
		else:
			player = CPUPlayer()
		player.name = name
		player.x = x
		player.y = y
		self.players.append(player)
	
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

	def check_collision(self, object):
		return object_hull_collision2d(self, object)

def object_bounding_sphere_collision(object1, object2):
	mesh1 = DataManager.meshes[object1.mesh]
	mesh2 = DataManager.meshes[object2.mesh]
	xdiff = (object1.x + mesh1.center.x) - (object2.x + mesh2.center.x)
	ydiff = (object1.y + mesh1.center.y) - (object2.y + mesh2.center.y)
	length = xdiff * xdiff + ydiff * ydiff
	if length < ((mesh1.radius * mesh1.radius) + (mesh2.radius * mesh2.radius)):
		return True
	else:
		return False

def object_hull_collision2d(object1, object2):
	mesh1 = DataManager.meshes[object1.mesh]
	mesh2 = DataManager.meshes[object2.mesh]
	xdiff = (object1.x + mesh1.hull.center.x) - (object2.x + mesh2.hull.center.x)
	ydiff = (object1.y + mesh1.hull.center.y) - (object2.y + mesh2.hull.center.y)
	
	# Get out before length calc if at all possible (sqrt, ** are expensive)
	if xdiff > (mesh1.hull.radius + mesh2.hull.radius) or ydiff > (mesh1.hull.radius + mesh2.hull.radius):
		return False
	
	radius1 = mesh1.hull.radius * mesh1.hull.radius
	radius2 = mesh2.hull.radius * mesh2.hull.radius
	length = xdiff * xdiff + ydiff * ydiff
	if length < radius1 + radius2:
		if hull_collision2d(mesh1.hull, object1, mesh2.hull, object2):
			return True
		else:
			return False
	else:
		return False

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
				d1 = (self.x - player.x) * (self.x - player.x) + \
					 (self.y - player.y) * (self.y - player.y)
				d2 = (newpos.x - player.x) * (newpos.x - player.x) + \
					 (newpos.y - player.y) * (newpos.y - player.y)
				if d2 < d1:
					old = Point2d(self.x, self.y)
					self.x = newpos.x
					self.y = newpos.y
					if self.check_collision(player):
						allow_x = False
						allow_y = False
					self.x = old.x
					self.y = old.y
		
		for item in level.items:
			d1 = (self.x - item.x) * (self.x - item.x) + \
				 (self.y - item.y) * (self.y - item.y)
			d2 = (newpos.x - item.x) * (newpos.x - item.x) + \
				 (newpos.y - item.y) * (newpos.y - item.y)
			if d2 < d1:
				old = Point2d(self.x, self.y)
				self.x = newpos.x
				self.y = newpos.y
				if self.check_collision(item):
					allow_x = False
					allow_y = False
				self.x = old.x
				self.y = old.y
		
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
class CPUPlayer(Player):
	def __init__(self):
		Player.__init__(self)

	def update(self, level):
		# See if we need to run away from any bombs!
		running = False
		bombs = []
		for item in level.items:
			if item.type == "Bomb":
				diffx = self.x - item.x
				diffy = self.y - item.y
				distance = diffx * diffx + diffy * diffy
				if distance < (item.radius * item.radius) + 0.1:
					bombs.append([item, distance])
					if self.x <= item.x:
						self.moving.left = True
					else:
						self.moving.right = True
					if self.y <= item.y:
						self.moving.down = True
					else:
						self.moving.up = True
					running = True
		
		# Select our closest target and go after her!
		if not running:
			chasing = False
			closest = [None, 1000]
			for player in level.players:
				if player != self:
					diffx = self.x - player.x
					diffy = self.y - player.y
					distance = diffx * diffx + diffy * diffy
					if distance < closest[1]:
						closest = [player, distance]
			player, distance = closest
			if player:
				chasing = True
				if self.x < player.x - 0.1:
					self.moving.right = True
					self.moving.left = False
				elif self.x > player.x + 0.1:
					self.moving.left = True
					self.moving.right = False
				else:
					self.moving.left = False
					self.moving.right = False
				if self.y < player.y - 0.1:
					self.moving.up = True
					self.moving.down = False
				elif self.y > player.y + 0.1:
					self.moving.down = True
					self.moving.up = False
				else:
					self.moving.down = False
					self.moving.up = False
				min_radius = DataManager.meshes[self.mesh].hull.radius + DataManager.meshes[player.mesh].hull.radius
				if distance < (min_radius * min_radius) + 0.1:
					level.add_bomb(self.x, self.y)
		
			if not chasing:
				self.moving.left = False
				self.moving.right = False
				self.moving.up = False
				self.moving.down = False
		
		Player.update(self, level)

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
				if xdiff * xdiff + ydiff * ydiff <= self.current_size * self.current_size:
					player.life -= 1
					if player.life == 0:
						Log.info("Killed " + player.name)
						del level.players[pos]
						if len(level.players) == 1:
							Log.info(level.players[0].name + " wins the match!")
							Event.post(Event.MATCH_WON)
					else:
						Log.info("Damaged " + player.name)
			
			for pos in range(len(level.items) - 1, -1, -1):
				item = level.items[pos]
				xdiff = item.x - self.x
				ydiff = item.y - self.y
				if xdiff * xdiff + ydiff * ydiff <= self.current_size * self.current_size:
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
				glutSolidSphere(self.current_size ** 3, 16, 16)
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
