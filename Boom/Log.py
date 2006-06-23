#!/usr/bin/env python

"""
	Log Module
	==========
		Logging and debug functions
	
		Using the Logger
		----------------
		Logging is extremely easy. The default log level is error, so you may
		want to lower it to debug while developing:
	
		>>> set_level("debug")
		log [x]: INFO Log level set to debug.
		>>> debug("This is a test.")
		<stdin> [1]: DEBUG This is a test.
		>>> critical("The reactor is melting!!!")
		<stdin> [1]: CRITICAL The reactor is melting!!!
	
		The default message format, as can be seen above, is:
	
		module [line number]: type message
	
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
		Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""

# Imports
import logging

# change this to change the format of log messages
LOG_FORMAT = "%(module)s [%(lineno)d]: %(levelname)s %(message)s"

# This will print error and critical messages by default
DEFAULT_LOG_LEVEL = logging.INFO

# get the root logger
logger = logging.getLogger('')

# setup a default handler if none exists
# if there are handlers, just use the first one we find
if logger.handlers == []:
	handler = logging.StreamHandler()
	handler.setFormatter(logging.Formatter(LOG_FORMAT))
	logger.addHandler(handler)
else:
	handler = logger.handlers[0]

# set the default log level
logger.setLevel(DEFAULT_LOG_LEVEL)
logger.info("Logging system started.")

# convenience functions
critical = logger.critical
error = logger.error
warning = logger.warning
info = logger.info
debug = logger.debug

add_handler = logger.addHandler			# could be used to save to files
remove_handler = logger.removeHandler	# and to stop saving to them :-D

def set_level(level):
	"""
	Set the log level. Valid levels are:
		- critical
		- error
		- warning
		- info
		- debug
	
	@type level: string
	@param level: The debug level, as explained above.
	"""
	levels = { "critical" 	: logging.CRITICAL,
			   "error"		: logging.ERROR,
			   "warning"	: logging.WARNING,
			   "info"		: logging.INFO,
			   "debug"		: logging.DEBUG }
	# make sure we have a valid level!
	if level in levels.keys():
		logger.setLevel(levels[level])
		logger.info("Log level set to " + level + ".")
	else:
		raise ValueError, level + " is not a valid logging level! Please pass critical, error, warning, info, or debug!"

def set_format(format):
	"""
	Set the log output format.
	
	@type format: string
	@param format: A format string, as defined by logging.Format
	"""
	handler.setFormatter(logging.Formatter(format))

def get_logger(name):
	"""
	Return a logger object. In most cases you should just use the convenience
	functions defined in this module to log messages instead of calling this
	function and storing your own logger object.
	
	@type name: string
	@param name: The name of the logging object to return.
	@rtype: logging.Logger
	@return: The new logger object.
	"""
	return logging.getLogger(name)
