#!/usr/bin/env python

"""
	Boom Data Manager
	=================
		An on-demand game data manager to load, store, and purge images,
		meshes, etc...
		
		Objects can be loaded manually by using the load function of the data
		stores, or it can be loaded automatically when access is attempted,
		so no explicit loading is necessary.
	
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
Log.info("Initializing game data manager")

import Graphics, VirtualFS, Sound

import os.path

#-------------------------------------------------------------------------------
class OnDemandLoader:
	"""
	On Demand Data Loader
	=====================
		A class that will act like a dictionary, but if a key is not found it
		attempts to load the key automatically. You can force load an item
		using the load function.
		
		This class is just a skeleton and should be inherited by classes that
		define the path and load function to actually do something.
	"""
	def __init__(self):
		self.data = {}
		self.path = ""
		self.extensions = []
	
	def __getitem__(self, key):
		# If they key is in our data store
		if key in self.data.keys():
			# Just return it
			return self.data[key]
		else:
			# Try to load the data!
			if VirtualFS.exists(os.path.join(self.path, key)):
				self.load(key)
				return self.data[key]
			else:
				Log.error("Failed to load " + os.path.join(self.path, key))
				return None
	
	def __setitem__(self, key, value):
		# Save the key/value into our data store
		self.data[key] = value
	
	def load(self, file):
		pass

#-------------------------------------------------------------------------------
class MeshLoader(OnDemandLoader):
	"""
	On Demand Mesh Loader
	=====================
		A class that will load meshes on demand from the Meshes directory in the
		virtual filesystem as they are needed by the engine.
	"""
	def __init__(self):
		OnDemandLoader.__init__(self)
		self.path = "Meshes"
	
	def load(self, key):
		"""
		Load a Mesh object into our data store.
		"""
		self.data[key] = Graphics.Mesh(os.path.join(self.path, key))
		Log.info("Loading " + key)

#-------------------------------------------------------------------------------
class SoundLoader(OnDemandLoader):
	"""
	On Demand Sound Loader
	======================
		A class that will load sounds on demand from the Sounds directory in the
		virtual filesystem as they are needed by the engine.
	"""
	def __init__(self):
		OnDemandLoader.__init__(self)
		self.path = "Sounds"
	
	def load(self, key):
		"""
		Load a sound object into our data store.
		"""
		self.data[key] = Sound.manager.load(key)

#-------------------------------------------------------------------------------

meshes = MeshLoader()
sounds = SoundLoader()
