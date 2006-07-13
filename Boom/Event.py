#!/usr/bin/env python

"""
	Boom Events
	===========
		Event definitions that are used by the engine's internal event queue. This
		module also manages the internal event queue.
	
		License
		-------
		Copyright (C) 2006 Daniel G. Taylor, Jens Taylor

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

MAX_EVENTS = 15
PRIORITY_HIGH = 0
PRIORITY_NORMAL = 1
PRIORITY_LOW = 2

EVENT_INITIALIZED = 0
EVENT_STATE_CHANGED = 1
EVENT_QUIT = 2

EVENT_LEVEL_LOADED = 3
EVENT_MATCH_WON = 4

EVENT_CAMERA_ROTATE = 5
EVENT_CAMERA_ZOOM = 6
EVENT_CAMERA_SHAKE = 7

queue = [[], [], []]
callbacks = {}

#-------------------------------------------------------------------------------
def register(event, callback):
	"""
	Register a callback function with the internal event handler.
	Any time that event is processed in the queue callback will be called.
	"""
	if not event in callbacks.keys():
		callbacks[event] = []
	callbacks[event].append(callback)

#-------------------------------------------------------------------------------
def unregister(event, callback):
	"""
	Unregister a callback function from the event handler so that it is no
	longer called on event.
	"""
	global callbacks
	for f in range(len(callbacks[event])):
		if callbacks[event][f] == callback:
			del callbacks[event][f]
			break
	if len(callbacks[event]) < 1:
		del callbacks[event]

#-------------------------------------------------------------------------------
def post(event, args = [], priority = PRIORITY_NORMAL):
	"""
	Post a new event to the internal event queue.
	"""
	queue[priority].append([event, args])

#-------------------------------------------------------------------------------
def handle_events():
	"""
	Handle the event queue. Process all high priority events, and then
	if there's still time (<= MAX_EVENTS), handle the normal and low 
	priority events.
	"""
	global queue
	processed = 0
	for priority in range(len(queue)):
		for event in range(len(queue[priority])):
			if priority == PRIORITY_HIGH or processed < MAX_EVENTS:
				if queue[priority][event][0] in callbacks.keys():
					for callback in callbacks[queue[priority][event][0]]:
						if len(queue[priority][event][1]) > 0:
							callback(queue[priority][event][1])
						else:
							callback()
					processed += 1
			else:
				del queue[priority][:event]
				return
		queue[priority] = []
	
	#queue = []
