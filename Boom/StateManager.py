#!/usr/bin/env python

"""
	Boom State Manager
	==================
		Manage game state, e.g. when when you are in a menu, playing, or paused.
		
		The game states are managed via a special stack. The top most item in the
		stack receives user input and has its update and draw methods called. The
		stack can be manipulated at any time through push, replace, pop, and clear.
		
		The GameState class can be inherited and its methods defined to create new
		game states.
		
		Usage example:
		
			>>>> push(PlayingState())
			StateManager [x]: INFO Changing game state to Playing
			>>>> push(PausedState())
			StateManager [x]: INFO Changing game state to Paused
			>>>> pop()
			StateManager [x]: INFO Changing game state to Playing
			>>>> replace(MainMenuState())
			StateManager [x]: INFO Chaning game state to Main Menu
	
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

import Log
Log.info("Initializing game state manager...")

import Objects
import DataManager
import Interface
import Keyboard
import Event

from Graphics import *

import sys

from copy import deepcopy

#-------------------------------------------------------------------------------
class GameState:
	def __init__(self):
		self.name = "Example"
		self.running = True
	
	def update(self):
		pass
	
	def draw(self):
		pass
	
	def key_pressed(self, key):
		pass
	
	def key_released(self, key):
		pass
	
	def mouse_clicked(self, button, x, y):
		pass
	
	def mouse_motion(self, x, y):
		pass

#-------------------------------------------------------------------------------
class MenuState(GameState):
	def __init__(self):
		GameState.__init__(self)
		self.name = "Menu"
	
	def key_pressed(key):
		pass

#-------------------------------------------------------------------------------
class PlayingState(GameState):
	def __init__(self):
		GameState.__init__(self)
		self.name = "Playing"
		self.level = None
	
	def key_pressed(self, key):
		if key == Keyboard.KEY_PAUSE:
			push(PausedState())
		elif key == Keyboard.KEY_MOVE_UP:
			self.level.player.moving.up = True
		elif key == Keyboard.KEY_MOVE_LEFT:
			self.level.player.moving.left = True
		elif key == Keyboard.KEY_MOVE_DOWN:
			self.level.player.moving.down = True
		elif key == Keyboard.KEY_MOVE_RIGHT:
			self.level.player.moving.right = True
		elif key == Keyboard.KEY_LAY_BOMB:
			if self.level.player.life:
				self.level.add_bomb(self.level.player.x, self.level.player.y)
		else:
			try:
				Log.info("Key pressed: " + chr(key))
			except ValueError:
				Log.info("Key pressed (keycode " + str(key) + ")")
	
	def key_released(self, key):
		if key == Keyboard.KEY_MOVE_UP:
			self.level.player.moving.up = False
		elif key == Keyboard.KEY_MOVE_LEFT:
			self.level.player.moving.left = False
		elif key == Keyboard.KEY_MOVE_DOWN:
			self.level.player.moving.down = False
		elif key == Keyboard.KEY_MOVE_RIGHT:
			self.level.player.moving.right = False
	
	def update(self):
		self.level.update()
	
	def draw(self):
		gluLookAt(0, -25, 25, \
				  0, 0, 0, \
				  0, 0, 1)
		self.level.draw()
	
	def load_level(self, level):
		self.level = Objects.Level(level)
		DataManager.meshes.load("bomb.obj")

#-------------------------------------------------------------------------------
class PausedState(GameState):
	def __init__(self):
		GameState.__init__(self)
		self.name = "Paused"
		self.menu = Interface.Menu("Paused")
		self.menu.add_item("Resume Game", self.resume)
		self.menu.add_item("Exit", self.exit)
		self.movement = deepcopy(current.level.player.moving)
	
	def resume(self):
		states[-2].level.player.moving = self.movement
		pop()
	
	def exit(self):
		Event.post(Event.QUIT)
		clear()
	
	def key_pressed(self, key):
		self.menu.key_pressed(key)
		
		if key == Keyboard.KEY_PAUSE:
			self.resume()
		elif key == Keyboard.KEY_MOVE_UP:
			self.movement.up = True
		elif key == Keyboard.KEY_MOVE_LEFT:
			self.movement.left = True
		elif key == Keyboard.KEY_MOVE_DOWN:
			self.movement.down = True
		elif key == Keyboard.KEY_MOVE_RIGHT:
			self.movement.right = True
	
	def key_released(self, key):
		if key == Keyboard.KEY_MOVE_UP:
			self.movement.up = False
		elif key == Keyboard.KEY_MOVE_LEFT:
			self.movement.left = False
		elif key == Keyboard.KEY_MOVE_DOWN:
			self.movement.down = False
		elif key == Keyboard.KEY_MOVE_RIGHT:
			self.movement.right = False
	
	def draw(self):
		states[-2].draw()
		glPushMatrix()
		glDisable(GL_DEPTH_TEST)
		glDisable(GL_LIGHTING)
		glEnable(GL_BLEND)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		glColor4f(0.0, 0.0, 0.0, 0.5)
		glBegin(GL_POLYGON)
		glVertex3f(-100, -100, 0)
		glVertex3f( 100, -100, 0)
		glVertex3f( 100,  100, 0)
		glVertex3f(-100,  100, 0)
		glEnd()
		glDisable(GL_BLEND)
		glColor4f(1.0, 1.0, 1.0, 1.0)
		self.menu.draw()
		glEnable(GL_LIGHTING)
		glEnable(GL_DEPTH_TEST)
		glPopMatrix()

#-------------------------------------------------------------------------------

states = []
current = None
on_state_change = None

def push(state):
	global current
	Log.info("Changing game state to " + state.name)
	states.append(state)
	current = state
	if on_state_change:
		on_state_change(current)

def replace(state):
	global current
	if len(states) < 1:
		Log.error("No game states present!")
		return
	Log.info("Changing game state to " + state.name)
	states[-1] = state
	current = state
	if on_state_change:
		on_state_change(current)

def pop():
	global current
	if len(states) < 1:
		Log.error("No game states present!")
		return
	
	del states[-1]
	
	if len(states) < 1:
		current = None
		if on_state_change:
			on_state_change(None)
	else:
		current = states[-1]
		Log.info("Changing game state to " + current.name)
		if on_state_change:
			on_state_change(current)

def clear():
	global current
	states = []
	current = None
	Log.info("Clearing game state stack...")
	if on_state_change:
		on_state_change(None)

def update():
	if len(states) < 1:
		Log.error("No game states present!")
		return
	if current and current.running:
		current.update()

def draw():
	if len(states) < 1:
		Log.error("No game states present!")
		return
	if current:
		current.draw()
