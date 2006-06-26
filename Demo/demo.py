#!/usr/bin/env python

"""
	Boom Demo
	=========
		Show what the Boom Engine can do! This demo loads a simple main menu and
		let's a player start a match against two computer-controlled players on
		a simple demo map.
		
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
		self.music = Boom.Sound.Music("Sounds/Menu.ogg")
		self.music.play()
	
	def start_demo(self):
		# Load a simple level and add a few players
		level = Boom.load_level("simple2")
		level.add_player("Daniel", 0, 0, True)
		level.add_player("CPU 1", 4, 1)
		level.add_player("CPU 2", -2, -4)
		level_music = Boom.Sound.Music("Sounds/Level.ogg")
		level_music.play()
	
	def match_won(self):
		Boom.StateManager.pop()
		self.music.play()
	
	def show_credits(self):
		pass
	
	def exit_demo(self):
		Boom.Event.post(Boom.Event.QUIT)
	
	def key_pressed(self, key):
		if key == 27:
			Boom.Event.post(Boom.Event.QUIT)
		self.menu.key_pressed(key)
	
	def update(self):
		pass
	
	def draw(self):
		self.menu.draw()

# Create an interface
interface = Boom.Interface.SDLInterface()

Boom.StateManager.push(MainMenuState())

# Start the engine
interface.start()
