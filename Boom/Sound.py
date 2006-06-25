#!/usr/bin/env python

"""
	Boom Sound Manager
	==================
		Manage the playback of sounds in the engine. The sound manager is abstracted
		so that any backend can be used. An SDL backend is provided by default, and
		it is enabled by default when using the engine's SDLInterface class.
		
		This module provides two classes to make playing sounds easy. They are the
		Sound and Music classes. Both have play and stop methods.
		
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
Log.info("Initializing sound manager...")

from Graphics import *

#-------------------------------------------------------------------------------
class Sound:
	"""
	Sound Object
	============
		Set and play/stop a sound file.
	"""
	def __init__(self, name):
		self.name = name
	
	def play(self):
		play(self.name)
	
	def stop(self):
		pass

#-------------------------------------------------------------------------------
class Music(Sound):
	"""
	Music Object
	============
		Set and play/stop streaming music.
	"""
	def play(self):
		play_music(self.name)
	
	def stop(self):
		stop_music()

#-------------------------------------------------------------------------------
class SDLSoundManager:
	"""
	SDL Sound Manager
	=================
		Manage and play sounds using SDL's mixer as the backend.
	"""
	def __init__(self):
		pass
	
	def load(self, name):
		return pygame.mixer.Sound(name)
	
	def play(self, name):
		pass
	
	def play_music(self, name):
		pygame.mixer.music.load(name)
		pygame.mixer.music.play(-1)
	
	def stop_music(self):
		pygame.mixer.music.stop()

#-------------------------------------------------------------------------------

manager = None

def load(name):
	if manager:
		return manager.load(name)
	else:
		Log.warning("No sound manager has been created...")

def play(name):
	if manager:
		manager.play(name)
	else:
		Log.warning("No sound manager has been created...")

def play_music(name):
	if manager:
		manager.play_music(name)
	else:
		Log.warning("No sound manager has been created...")

def stop_music():
	if manager:
		manager.stop_music()
	else:
		Log.warning("No sound manager has been created...")
