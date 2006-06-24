#!/usr/bin/env python

# Setup python's search path so we can run from svn checkout
import os, sys
sys.path.append(os.getcwd() + "/../")

import Boom

class MainMenuState(Boom.StateManager.GameState):
	def __init__(self):
		Boom.StateManager.GameState.__init__(self)
		self.name = "Main Menu"
		self.menu = Boom.Interface.Menu("Boom Demo")
		self.menu.add_item("Start Demo", self.start_demo)
		#self.menu.add_item("Credits", self.show_credits)
		self.menu.add_item("Exit", self.exit_demo)
		Boom.Event.register(Boom.Event.MATCH_WON, self.match_won)
	
	def start_demo(self):
		# Load a simple level and add a few players
		level = Boom.load_level("simple2")
		level.add_player("Daniel", 0, 0, True)
		level.add_player("CPU 1", 4, 1)
		level.add_player("CPU 2", -2, -4)
	
	def match_won(self):
		Boom.StateManager.pop()
	
	def show_credits(self):
		pass
	
	def exit_demo(self):
		Boom.Event.post(Boom.Event.QUIT)
	
	def key_pressed(self, key):
		self.menu.key_pressed(key)
	
	def update(self):
		pass
	
	def draw(self):
		Boom.Graphics.gluLookAt(0, -25, 25, \
				  0, 0, 0, \
				  0, 0, 1)
		self.menu.draw()

# Create an interface
interface = Boom.Interface.SDLInterface()

Boom.StateManager.push(MainMenuState())

# Start the engine
interface.start()
