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
import Event
import Keyboard

from Graphics import *

from time import time, sleep

tdiff = 0
last_time = 0

MENU_SUBMENU = 0
MENU_ITEM = 1

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
		"""
		Initialize OpenGL. Sets up OpenGL features and settings.
		"""
		# Initialize GLUT
		glutInit(sys.argv)
		
		# Set the background color, enable shading and depth testing, etc...
		glClearColor(0.0, 0.0, 0.0, 0.0)
		glClearDepth(1.0)
		glDepthFunc(GL_LEQUAL)
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
		glEnable(GL_DEPTH_TEST)
		
		# Use smooth shading
		glShadeModel(GL_SMOOTH)
		
		# Enable textures
		#glEnable(GL_COLOR_MATERIAL)
		#glEnable(GL_TEXTURE_2D)
		
		# Enable backface culling
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
		"""
		Clear the screen and draw the current state.
		"""
		global last_time
		global tdiff
		
		# Calculate the time between frames
		cur_time = time()
		tdiff = cur_time - last_time
		last_time = cur_time

		# Clear the frame and draw the current state
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
		"""
		Resize the window.
		"""
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
		"""
		Clean up and exit the game
		"""
		Log.info("Shutting down...")

#-------------------------------------------------------------------------------
class SDLInterface(BaseInterface):
	"""
	An SDL Interface
	================
		A class that creates an OpenGL-capable SDL window and dispatches user events
		to the proper handlers in the state manager.
	"""
	def __init__(self, width = 640, height = 480):
		self.init_sdl(width, height)
		BaseInterface.__init__(self)
	
	def init_sdl(self, width, height):
		"""
		Initialize SDL and create a window.
		"""
		# Initialize SDL
		pygame.init()
		
		# Create a new OpenGL capable window
		flags = HWSURFACE | DOUBLEBUF | OPENGL
		self.screen = pygame.display.set_mode([width, height], flags)
		self.resize(width, height)
	
	def start(self):
		"""
		Start the game engine.
		"""
		# Start the main event loop
		while 1:
			# Process the internal event queue
			for event in Event.queue:
				if event in Event.callbacks.keys():
					for callback in Event.callbacks[event]:
						callback()
				if event == Event.QUIT:
					return self.shutdown()
			Event.queue = []
			
			# Process the SDL event queue
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
			
			# Update and draw the current state
			StateManager.update()
			self.draw()
	
	def flip(self):
		"""
		Flip (draw) the display.
		"""
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
		self.title_color = Color(1.0, 1.0, 1.0)
		self.color = Color(1.0, 1.0, 1.0)
		self.selected_color = Color(1.0, 0.0, 0.0)
		self.selected = 0
	
	def add_item(self, label, callback, submenu = False):
		"""
		Add an item or submenu to the menu.
		"""
		if not submenu:
			self.items.append([label, callback, MENU_ITEM])
		else:
			self.items.append([label, callback, MENU_SUBMENU])
	
	def key_pressed(self, key):
		"""
		Handle keyboard input (e.g. to move the current selection pointer)
		"""
		if key == Keyboard.KEY_SELECT:
			self.items[self.selected][1]()
		elif key == Keyboard.KEY_UP:
			self.up()
		elif key == Keyboard.KEY_DOWN:
			self.down()
	
	def up(self):
		"""
		Select one item up, looping around if needed.
		"""
		if self.selected > 0:
			self.selected -= 1
		else:
			self.selected = len(self.items) - 1
	
	def down(self):
		"""
		Select one item down, looping around if needed.
		"""
		if self.selected < len(self.items) - 1:
			self.selected += 1
		else:
			self.selected = 0
	
	def draw(self):
		"""
		Draw the menu centered on the screen.
		"""
		glPushMatrix()
		glDisable(GL_LIGHTING)
		y = 0
		# Draw the menu title
		glColor3fv(self.title_color.array())
		glRasterPos2f(-1, y)
		y -= 1
		for character in self.title:
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(character))
		pos = 0
		# Draw each menu item
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
		glEnable(GL_LIGHTING)
		glPopMatrix()
