#!/usr/bin/env python

"""
	Boom Virtual Filesystem
	=======================
		Manage an internal virtual filesystem that contains all game data. Directories
		and compressed archives can be mounted and accessed through this system. File-
		like objects are returned for reading/writing. Compression is handled on the
		fly.
		
		Usage example:
		
			>>>> mount("/usr/share/game.tar.bz2")
			>>>> data = open("test.txt")
			>>>> print data.read()
		
		The above mounts the archive and prints out the contents of the file test.txt.
		
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

import os, os.path, tarfile

import Log
Log.info("Initializing virtual filesystem...")

TYPE_DIRECTORY = 0
TYPE_BZIP_TAR = 1

#-------------------------------------------------------------------------------
class MountPoint:
	def __init__(self, path = "", type = TYPE_DIRECTORY):
		self.path = path
		self.type = type
	
	def __repr__(self):
		"""
		Represent the mount point as [path, type]
		"""
		if self.type == TYPE_DIRECTORY:
			type = "Directory"
		elif self.type == TYPE_BZIP_TAR:
			type = "Bzipped Tar Archive"
		return "[" + self.path + ", " + type + "]"

#-------------------------------------------------------------------------------
def mount(location):
	"""
	Mount a location, such as a directory or compressed archive.
	"""
	if location[-1] == "/":
		location = location[:-1]
	Log.info("Mounting " + location)
	if os.path.isdir(location):
		mtab.append(MountPoint(location))
	elif os.path.isfile(location):
		mpoint = MountPoint(location, TYPE_BZIP_TAR)
		mpoint.data = tarfile.open(location, "r:bz2")
		mtab.append(MountPoint(location, TYPE_BZIP_TAR))

#-------------------------------------------------------------------------------
def umount(location):
	"""
	Unmount a location that was previously mounted.
	"""
	found = False
	for pos in range(len(mtab)):
		if mtab[pos].path == location:
			Log.info("Unmounting " + location)
			del mtab[pos]
			found = True
			break
	if not found:
		Log.warning("Mount point was not found!")
	return found

#-------------------------------------------------------------------------------
def open(filename):
	"""
	Open and return a file-like object from the path given, relative to the
	mount root.
	"""
	for location in mtab:
		if location.type == TYPE_DIRECTORY:
			path = os.path.join(location.path, filename)
			if os.path.exists(path):
				return file(path)
		elif location.type == TYPE_BZIP_TAR:
			return location.data.extractfile(filename)
	Log.error("Unable to find file " + filename)
	return None

#-------------------------------------------------------------------------------
def listdir(directory):
	"""
	List the contents of a directory.
	"""
	files = []
	for location in mtab:
		if location.type == TYPE_DIRECTORY:
			path = os.path.join(location.path, directory)
			if os.path.exists(path):
				if os.path.isdir(path):
					files += os.listdir(path)
				else:
					Log.error("Path is not a directory...")
					return None
		elif location.type == TYPE_BZIP_TAR:
			pass
	return files

#-------------------------------------------------------------------------------
def exists(filename):
	"""
	Check if a file exists.
	"""
	for location in mtab:
		if location.type == TYPE_DIRECTORY:
			if os.path.exists(os.path.join(location.path, filename)):
				return True
		elif location.type == TYPE_BZIP_TAR:
			pass
	return False

#-------------------------------------------------------------------------------
# Keep track of all mount points
mtab = []

# Mount the current working directory by default
mount(os.getcwd())
