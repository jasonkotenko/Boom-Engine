#!/usr/bin/env python

"""
	Boom Game Engine
	================
		A game engine to create Bomberman-like games.
	
		Modules
		-------
		Log			 - Generic logging facility
		Event		 - Event handling and dispatching
		VirtualFS	 - Virtual filesystem used within the engine
		DataManager	 - Loads and stores game data such as images and meshes
		Graphics	 - 2d/3d game object classes (e.g. Point2d, Mesh, Material)
					   and OpenGL/SDL functions
		Objects		 - Game object classes (e.g. Level, Player, Bomb)
		StateManager - Game state manager
		Keyboard	 - Defined key constants and keyboard functions
		Interface	 - User interface classes
		Sound		 - Manage sound playback
		
		A Simple Example
		----------------
		import Boom
		
		interface = Boom.Interface.SDLInterface()
		Boom.load_level("level_name")
		interface.start()
	
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

Event = None
VirtualFS = None
DataManager = None
Graphics = None
Objects = None
StateManager = None
Interface = None
Sound = None

version = 0.1

def init():
	global Event
	global VirtualFS
	global DataManager
	global Graphics
	global Objects
	global StateManager
	global Interface
	global Sound
	
	import Event
	import VirtualFS
	import DataManager
	import Graphics
	import Objects
	import StateManager
	import Interface
	import Sound

def load_level(level):
	"""
	A convienience function that will change the game state to playing and load a
	specified level.
	
	@return: The level that was loaded.
	"""
	# Change the game state if we need to
	if StateManager.current == None or StateManager.current.name != "Playing":
		StateManager.push(StateManager.PlayingState())
		
	# Load the level
	StateManager.current.load_level(level)
	
	return StateManager.current.level
