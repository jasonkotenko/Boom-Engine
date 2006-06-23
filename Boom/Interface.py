#!/usr/bin/env python

"""
	Boom Interface
	==============
		Classes for handling interface objects and initializing and displaying an
		OpenGL window in which the game will be rendered.
		
		This module also manages the internal event queue, which can be added to
		using post_event.
	
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
import Objects
import StateManager

from Graphics import *
from Keyboard import *
from Event import *

from time import time, sleep

tdiff = 0
last_time = 0

MENU_SUBMENU = 0
MENU_ITEM = 1

event_queue = []

#-------------------------------------------------------------------------------
class BaseInterface:
	"""
	Base Interface
	==============
		A class that initializes OpenGL, handles drawing and resizing of the
		window, etc.
		
		This class should be inherited by classes that define the start, flip, and
		queue_flip functions!
	"""
	def __init__(self):
		self.init_gl()
	
	def start(self):
		pass
	
	def init_gl(self, width = 640, height = 480):
		# Set the background color, enable shading and depth testing, etc...
		glClearColor(0.0, 0.0, 0.0, 0.0)
		glClearDepth(1.0)
		glDepthFunc(GL_LEQUAL)
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
		glEnable(GL_DEPTH_TEST)
		
		glShadeModel(GL_SMOOTH)
		#glEnable(GL_COLOR_MATERIAL)
		#glEnable(GL_TEXTURE_2D)
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE)
		
		glCullFace(GL_BACK)
		glEnable(GL_CULL_FACE)
		
		# Setup and enable lighting
		glLightfv(GL_LIGHT1, GL_AMBIENT, (0.2, 0.2, 0.2, 1.0))
		glLightfv(GL_LIGHT1, GL_DIFFUSE, (1.0, 1.0, 1.0, 1.0))
		glLightfv(GL_LIGHT1, GL_SPECULAR, (1.0, 1.0, 1.0, 1.0))
		glLightfv(GL_LIGHT1, GL_POSITION, (25.0, -25.0, 25.0, 1.0))
		glEnable(GL_LIGHT1)
		glEnable(GL_LIGHTING)
		glEnable(GL_NORMALIZE)
	
	def draw(self):
		global last_time
		global tdiff
		cur_time = time()
		tdiff = cur_time - last_time
		last_time = cur_time

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		glLoadIdentity()
		StateManager.draw()
		self.flip()
		sleep(0.001)
	
	def flip(self):
		pass
	
	def queue_flip(self):
		pass
	
	def resize(self, width, height):
		# Prevent zero heights... weird shit I tell you
		if height == 0:
			height = 1
		
		# Reset the camer/view to the new width/height
		glViewport(0, 0, width, height)
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		gluPerspective(25.0, float(width)/float(height), 0.1, 1000.0)
		glMatrixMode(GL_MODELVIEW)
	
	def shutdown(self):
		Log.info("Shutting down...")
		sys.exit(0)

#-------------------------------------------------------------------------------
class SDLInterface(BaseInterface):
	"""
	An SDL Interface
	================
		A class that creates an OpenGL-capable SDL window and dispatches user events
		to the proper handlers in the state manager.
	"""
	def __init__(self):
		self.init_sdl()
		glutInit(sys.argv)
		BaseInterface.__init__(self)
	
	def init_sdl(self, width = 640, height = 480):
		pygame.init()
		
		flags = HWSURFACE | DOUBLEBUF | OPENGL
		self.screen = pygame.display.set_mode([width, height], flags)
		self.resize(width, height)
	
	def start(self):
		global event_queue
		while 1:
			for event in event_queue:
				if event == EVENT_QUIT:
					self.shutdown()
			event_queue = []
			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					self.shutdown()
				elif event.type == pygame.VIDEORESIZE:
					self.resize(event.w, event.h)
				elif event.type == pygame.KEYDOWN:
					StateManager.current.key_pressed(event.key)
					self.queue_flip()
				elif event.type == pygame.KEYUP:
					StateManager.current.key_released(event.key)
			StateManager.update()
			self.draw()
	
	def flip(self):
		pygame.display.flip()

#-------------------------------------------------------------------------------
class Menu:
	"""
	Menu System
	===========
		A simple vertical menu system, drawn using bitmapped fonts. Menus can have
		items and submenus which in turn are just more Menu objects.
		
		Items can have callbacks defined that are called when the item is selected.
	"""
	def __init__(self, title = "Default Menu Title"):
		self.title = title
		self.items = []
		self.color = Color(1.0, 1.0, 1.0)
		self.selected_color = Color(1.0, 0.0, 0.0)
		self.selected = 0
	
	def add_item(self, label, callback, submenu = False):
		if not submenu:
			self.items.append([label, callback, MENU_ITEM])
		else:
			self.items.append([label, callback, MENU_SUBMENU])
	
	def key_pressed(self, key):
		if key == KEY_SELECT:
			self.items[self.selected][1]()
		elif key == KEY_UP:
			self.up()
		elif key == KEY_DOWN:
			self.down()
	
	def up(self):
		if self.selected > 0:
			self.selected -= 1
		else:
			self.selected = len(self.items) - 1
	
	def down(self):
		if self.selected < len(self.items) - 1:
			self.selected += 1
		else:
			self.selected = 0
	
	def draw(self):
		y = 0
		glRasterPos2f(-1, y)
		y -= 1
		for character in self.title:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(character))
		pos = 0
		for label, callback, type in self.items:
			if pos == self.selected:
				glColor3fv(self.selected_color.array())
			else:
				glColor3fv(self.color.array())
			glRasterPos2f(-1, y)
			y -= 0.7
			for character in label:
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ord(character))
			pos += 1


def post_event(event):
	event_queue.append(event)
