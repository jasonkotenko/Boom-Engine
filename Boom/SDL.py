#!/usr/bin/env python

# SDL functions
try:
	from pygame import *
except:
	import sys
	import Log
	Log.critical("Can't import SDL bindings...")
	Log.critical("Please install them from http://www.pygame.org/")
	sys.exit(1)
