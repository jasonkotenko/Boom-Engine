#!/usr/bin/env python

"""
	Boom Game Engine
	================
		A game engine to create Bomberman-like games.
	
		Modules
		-------
		Log			 - Generic logging facility
		VirtualFS	 - Virtual filesystem used within the engine
		DataManager	 - Loads and stores game data such as images and meshes
		Graphics	 - 2d/3d game object classes (e.g. Point2d, Mesh, Material)
					   and OpenGL/SDL functions
		Objects		 - Game object classes (e.g. Level, Player, Bomb)
		StateManager - Game state manager
		Keyboard	 - Defined key constants and keyboard functions
		Interface	 - User interface classes
		
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
import VirtualFS
import DataManager
import Graphics
import Objects
import StateManager
import Interface

version = 0.1

def load_level(level):
	StateManager.push(StateManager.PlayingState())
	StateManager.current.load_level(level)
	return StateManager.current.level
