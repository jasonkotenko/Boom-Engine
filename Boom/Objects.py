#!/usr/bin/env python

"""
	Boom Game Objects
	=================
		Classes to define game entities such as a level, player, item, bomb, etc. This
		is the meat of the actual game.
	
		License
		-------
		Copyright (C) 2006 Daniel G. Taylor, Jason F. Kotenko, Jens Taylor

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

from math import sin, asin, sqrt, degrees, radians, pi, atan2
from time import time

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
		self.explosion_last = 0.0
		self.explosion_counter = 1
		self.explosion_links = []
	
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
	
	def get_angle(self):
		if self.up and self.left:
			angle = 135
		elif self.up and self.right:
			angle = 45
		elif self.down and self.left:
			angle = 225
		elif self.down and self.right:
			angle = 315
		elif self.up:
			angle = 90
		elif self.down:
			angle = 270
		elif self.left:
			angle = 180
		elif self.right:
			angle = 0
		try:
			return radians(angle), True
		except:
			return None, False

#-------------------------------------------------------------------------------
class GameObject:
	def __init__(self, mesh = None, x = 0, y = 0):
		self.id = 0
		self.mesh = mesh
		self.x = x
		self.y = y
		self.motion = PolarVector2d(None, 1.0)
		self.motion.moving = False
		self.scale = 1.0
	
	def update(self, level):
		if self.motion.moving:
			oldpos = Point2d(self.x, self.y)
			self.x += cos(self.motion.angle) * self.motion.radius * Interface.tdiff
			self.y += sin(self.motion.angle) * self.motion.radius * Interface.tdiff
			if self.check_collisions(level, oldpos):
				self.x, self.y = oldpos.x, oldpos.y
		return True
	
	def draw(self):
		pass
	
	def check_collisions(self, level, oldpos):
		for player in level.players:
			if player != self:
				d1 = (oldpos.x - player.x) * (oldpos.x - player.x) + \
					 (oldpos.y - player.y) * (oldpos.y - player.y)
				d2 = (self.x - player.x) * (self.x - player.x) + \
					 (self.y - player.y) * (self.y - player.y)
				if d2 < d1:
					if self.hull_collision2d(player):
						return True
		
		for item in level.items:
			if item != self:
				d1 = (oldpos.x - item.x) * (oldpos.x - item.x) + \
					 (oldpos.y - item.y) * (oldpos.y - item.y)
				d2 = (self.x - item.x) * (self.x - item.x) + \
					 (self.y - item.y) * (self.y - item.y)
				if d2 < d1:
					if self.hull_collision2d(item):
						return True
		return False
	
	def hull_collision2d(self, object):
		mesh1 = DataManager.meshes[self.mesh]
		mesh2 = DataManager.meshes[object.mesh]
		xdiff = (self.x + mesh1.hull.center.x) - (object.x + mesh2.hull.center.x)
		ydiff = (self.y + mesh1.hull.center.y) - (object.y + mesh2.hull.center.y)
		
		# Get out before length calc if at all possible (sqrt, ** are expensive)
		if xdiff > (mesh1.hull.radius + mesh2.hull.radius) or ydiff > (mesh1.hull.radius + mesh2.hull.radius):
			return False
	
		radius1 = mesh1.hull.radius * mesh1.hull.radius
		radius2 = mesh2.hull.radius * mesh2.hull.radius
		length = xdiff * xdiff + ydiff * ydiff
		if length < radius1 + radius2:
			if hull_collision2d(mesh1.hull, self, mesh2.hull, object):
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
		self.motion.radius = 3.0
		self.life = 1
		self.mesh = "player.obj"
	
	def update(self, level):
		return GameObject.update(self, level)
		
	def draw(self):
		glPushMatrix()
		glTranslatef(self.x, self.y, 0.0)
		if self.motion.angle != None:
			glRotatef(degrees(self.motion.angle + (pi / 2.0)), 0, 0, 1)
		DataManager.meshes[self.mesh].render()
		glPopMatrix()

#-------------------------------------------------------------------------------
class CPUPlayer(Player):
	def __init__(self):
		Player.__init__(self)
		# Set whether this player thinks (enable/disable artificial intelligence)
		self.thinks = True

	def update(self, level):
		# See if we need to run away from any bombs!
		running = False
		self.motion.moving = False
		bombs = []
		for item in level.items:
			if item.type == "Bomb":
				diffx = self.x - item.x
				diffy = self.y - item.y
				distance = diffx * diffx + diffy * diffy
				if distance < (item.radius * item.radius) + 0.1:
					bombs.append([item, distance])
					self.motion.angle = atan2(diffy, diffx)
					self.motion.moving = True
					running = True
		
		# Select our closest target and go after her!
		if not running and self.thinks:
			chasing = False
			closest = [None, 1000]
			for player in level.players:
				if player != self:
					diffx = self.x - player.x
					diffy = self.y - player.y
					distance = diffx * diffx + diffy * diffy
					if distance < closest[1]:
						closest = [player, distance, diffx, diffy]
			player, distance, diffx, diffy = closest
			if player:
				chasing = True
				self.motion.angle = atan2(-diffy, -diffx)
				self.motion.moving = True
				min_radius = DataManager.meshes[self.mesh].hull.radius + DataManager.meshes[player.mesh].hull.radius
				if distance < (min_radius * min_radius) + 0.1:
					level.add_bomb(self.x, self.y)
		
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
		retval = GameObject.update(self, level)
		self.timer -= Interface.tdiff
		if self.timer <= 0:
			retval = self.timeout(level)
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
	
	def timeout(self, level):
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
		self.explosion_index = -1
		self.hit_bomb = False
	
	def update(self, level):
		if not self.exploding:
			return Item.update(self, level)
		else:
			size_diff = (self.radius - self.current_size)
			self.current_size += Interface.tdiff * (size_diff + 0.5) * 2
			if self.current_size >= self.radius:
				if not self.hit_bomb:
					level.explosion_links[self.explosion_index] = 0
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
							Event.post(Event.EVENT_MATCH_WON)
					else:
						Log.info("Damaged " + player.name)
			
			for pos in range(len(level.items) - 1, -1, -1):
				item = level.items[pos]
				xdiff = item.x - self.x
				ydiff = item.y - self.y
				if xdiff * xdiff + ydiff * ydiff <= self.current_size * self.current_size:
					if item.type == "Bomb":
						if item is not self and not item.exploding:
							self.hit_bomb = True
							item.timeout(level, self.explosion_index)
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
	
	def timeout(self, level, linkindex = -1):
		self.exploding = True
		old_time = level.explosion_last
		level.explosion_last = time()
		if level.explosion_last - old_time <= .15:
			level.explosion_counter += 1
		else:
			level.explosion_counter = 1
		if level.explosion_counter > 4:
			Event.post(Event.EVENT_CAMERA_SHAKE)
			#self.radius *= 2.0
		self.explosion_index = linkindex
		if linkindex == -1:
			for links in range(len(level.explosion_links)):
				if level.explosion_links[links] == 0:
					level.explosion_links[links] = 1
					self.explosion_index = links
					break
			if self.explosion_index == -1:
				level.explosion_links.append(1)
				self.explosion_index = len(level.explosion_links) - 1
		else:
			level.explosion_links[self.explosion_index] += 1
			if level.explosion_links[self.explosion_index] > 4:
				Event.post(Event.EVENT_CAMERA_SHAKE)
				self.radius *= 2.0
		print self.explosion_index
		return True
