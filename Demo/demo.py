#!/usr/bin/env python

"""
	Boom Demo
	=========
		Show what the Boom Engine can do! This demo loads a simple main menu and
		let's a player start a match against two computer-controlled players on
		a simple demo map.
		
		Command-Line options:
			--nosound	turns off music and sounds
		
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

# Setup python's search path so we can run from svn checkout
import os, sys
sys.path.append(os.getcwd() + "/../")

# Parse command line options (so far only option for no sounds)
nosound = False
noai = False
for x in sys.argv[1:]:
	if x == "--nosound" or x == "--no-sound":
		nosound = True
	elif x == "--no-ai":
		noai = True

import Boom

# Initialize Boom
Boom.init()

class MainMenuState(Boom.StateManager.GameState):
	def __init__(self):
		Boom.StateManager.GameState.__init__(self)
		self.name = "Main Menu"
		self.menu = Boom.Interface.Menu("Boom Demo")
		self.menu.add_item("Start Demo", self.start_demo)
		self.menu.add_item("Camera Demo", self.camera_demo)
		#self.menu.add_item("Credits", self.show_credits)
		self.menu.add_item("Exit", self.exit_demo)
		Boom.Event.register(Boom.Event.MATCH_WON, self.match_won)
		if not nosound:
			self.music = Boom.Sound.Music("Sounds/Menu.ogg")
			self.music.play()
	
	def start_demo(self):
		# Load a simple level and add a few players
		level = Boom.load_level("simpleplane")
		level.add_player("Daniel", -1, 0, True)
		level.add_player("CPU 1", 4, 1)
		level.add_player("CPU 2", -2, -4)
		if noai:
			level.players[1].thinks = False
			level.players[2].thinks = False
		if not nosound:
			level_music = Boom.Sound.Music("Sounds/Level.ogg")
			level_music.play()

	def camera_demo(self):
		Boom.StateManager.push(CameraDemo())
	
	def match_won(self):
		Boom.StateManager.pop()
		if not nosound:
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

class CameraDemo(Boom.StateManager.GameState):
	def __init__(self):
		Boom.StateManager.GameState.__init__(self)
		self.name = "Camera Demo"
		self.camera = Boom.Camera.CubeCamera(0, 25.0, 10.0, 0)

	def update(self):
		self.camera.update()

	def draw(self):
		self.camera.draw()
		Boom.Graphics.glColor3f(1.0, 1.0, 1.0)
		Boom.Graphics.glutSolidCube(10.0)

	def key_pressed(self, key):
		if key == ord("1"):
			self.camera.rotate(.8, self.camera.FACE1)
		elif key == ord("2"):
			self.camera.rotate(.8, self.camera.FACE2)
		elif key == ord("3"):
			self.camera.rotate(.8, self.camera.FACE3)
		elif key == ord("4"):
			self.camera.rotate(.8, self.camera.FACE4)
		elif key == ord("5"):
			self.camera.rotate(.8, self.camera.FACE5)
		elif key == ord("6"):
			self.camera.rotate(.8, self.camera.FACE6)
		elif key == Boom.Keyboard.KEY_PAUSE:
			Boom.StateManager.pop()
		elif key == Boom.Keyboard.KEY_MOVE_UP:
			self.camera.zoom(.8, self.camera.zooms[1] - 30)
		elif key == Boom.Keyboard.KEY_MOVE_DOWN:
			self.camera.zoom(.8, self.camera.zooms[1] + 30)

# Create an interface
interface = Boom.Interface.SDLInterface(640, 480)

Boom.StateManager.push(MainMenuState())

# Start the engine
interface.start()
