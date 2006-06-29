#!/usr/bin/env python

"""
	Boom Events
	===========
		Event definitions that are used by the engine's internal event queue. This
		module also manages the internal event queue.
	
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
Log.info("Initializing internal event handler...")

INITIALIZED = 0
QUIT = 1

LEVEL_LOADED = 2
MATCH_WON = 3

queue = []
callbacks = {}

def register(event, callback):
	if not event in callbacks.keys():
		callbacks[event] = []
	callbacks[event].append(callback)

def post(event):
	"""
	Post a new event to the internal event queue.
	"""
	queue.append(event)

def handle_events():
	global queue
	for event in queue:
		if event in callbacks.keys():
			for callback in callbacks[event]:
				callback()
	queue = []
