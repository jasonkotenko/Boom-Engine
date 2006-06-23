#!/usr/bin/env python

"""
	Boom Graphics Objects
	=====================
		This module defines classes for graphical objects such as points,
		images, materials, meshes, as well as providing access to OpenGL
		and SDL to the rest of the engine.
	
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

import os, os.path, sys

import Log
Log.info("Initializing graphics subsystem...")

import VirtualFS

# OpenGL functions
try:
	from OpenGL.GL import *
	from OpenGL.GLU import *
	from OpenGL.GLUT import *
except:
	Log.critial("Can't import OpenGL bindings...")
	Log.critical("Please install them from http://pyopengl.sourceforge.net/")
	sys.exit(1)

# SDL functions
try:
	import pygame
	from pygame.locals import *
except:
	Log.critical("Can't import SDL bindings...")
	Log.critical("Please install them from http://www.pygame.org/")
	sys.exit(1)

#-------------------------------------------------------------------------------
class Point2d:
	"""
	Two-Dimensional Point
	=====================
		Stores a point in two-dimensional space. Coordinates can be accessed
		through x and y members.
		
		Usage example:
		
			>>>> test = Point2d(0.2, 25)
			>>>> print test
			[0.2, 25]
			>>>> print test.x
			0.2
	"""
	def __init__(self, x = 0, y = 0):
		self.x = x
		self.y = y
	
	def __repr__(self):
		return "[" + str(self.x) + ", " + str(self.y) + "]"

#-------------------------------------------------------------------------------
class Point3d:
	"""
	Three-Dimensional Point
	=======================
		Stores a point in three-dimensional space. Coordinates can be accessed
		through x, y, and z members.
		
		Usage example:
		
			>>>> test = Point3d(3, 5, 7)
			>>>> print test
			[3, 5, 7]
			>>>> print test.z
			7
	"""
	def __init__(self, x = 0, y = 0, z = 0):
		self.x = x
		self.y = y
		self.z = z
	
	def __repr__(self):
		return "[" + str(self.x) + ", " + str(self.y) + ", " + str(self.z) + "]"

#-------------------------------------------------------------------------------
class Color:
	"""
	Color Value
	===========
		Stores information about a color's red, green, blue, and alpha values.
		
		Usage example:
		
			>>>> test = Color(0.5, 0.25, 0.75)
			>>>> print test
			[0.5, 0.25, 0.75, 1.0]
			>>>> print test.red, test.alpha
			0.5, 1.0
	"""
	def __init__(self, red = 1.0, green = 1.0, blue = 1.0, alpha = 1.0):
		self.red = red
		self.green = green
		self.blue = blue
		self.alpha = alpha
	
	def array(self):
		return [self.red, self.green, self.blue, self.alpha]
	
	def ___repr___(self):
		return "[" + str(self.red) + ", " + str(self.green) + ", " + str(self.blue) +\
			   ", " + str(self.alpha) + "]"

#-------------------------------------------------------------------------------
class Image:
	"""
	Image
	=====
		Stores information about an image.
	"""
	def __init__(self):
		self.width = 0
		self.height = 0
		self.data = ""
	
	def load(self, filename):
		Log.debug("Loading " + filename)
		self.width = 0
		self.height = 0
		self.data = []
		if filename[-4:] == ".ppm":
			self.load_ppm(VirtualFS.open(filename))
		else:
			Log.error("Uknown image format!")
	
	def load_ppm(self, data):
		data2 = []
		for line in data:
			if line[0] != "#":
				for part in line.split("\t"):
					for piece in part.split(" "):
						if piece and ord(piece[0]) >= 48:
							data2.append(piece.strip())
		if data2[0] != "P3":
			Log.warning("PPM Header not found...")
			return False
		self.width = int(data2[1])
		self.height = int(data2[2])
		for y in range(self.height):
			self.data.append([])
		maxvalue = int(data2[3])
		pos = 4
		for y in range(self.height):
			for x in range(self.width):
				self.data[y].append([float(data2[pos]) / maxvalue,
								   float(data2[pos + 1]) / maxvalue,
								   float(data2[pos + 2]) / maxvalue])
				pos += 3
		return True

#-------------------------------------------------------------------------------
class Material:
	"""
	Material Properties
	===================
		Stores information about a material, including ambient, diffuse, and
		specular colors and texture info.
	"""
	def __init__(self, name = "New Material"):
		self.name = name
		self.ambient = Color(0.2, 0.2, 0.2)
		self.diffuse = Color(0.8, 0.8, 0.8)
		self.specular = Color(1.0, 1.0, 1.0)
		self.alpha = 1.0
		self.illumination = 0.0
		self.shininess = 0.0
		self.texture = None
	
	def set(self):
		glMaterialfv(GL_FRONT, GL_AMBIENT, self.ambient.array())
		glMaterialfv(GL_FRONT, GL_DIFFUSE, self.diffuse.array())
		glMaterialfv(GL_FRONT, GL_SPECULAR, self.specular.array())
		glMateriali(GL_FRONT, GL_SHININESS, self.shininess)
		#glBindTexture(GL_TEXTURE_2D, texture)

#-------------------------------------------------------------------------------
class Vertex2d:
	"""
	Two-Dimensional Vertex
	======================
		Stores information about a two dimensional vertex, such as its position,
		normal vector, and texture (UV) coordinates.
	"""
	def __init__(self, x = 0, y = 0):
		self.pos = Point2d(x, y)
		self.normal = None
		self.texture_coord = None
	
	def __repr__(self):
		return self.pos + ", " + self.normal + ", " + self.texture_coord

#-------------------------------------------------------------------------------
class Vertex3d:
	"""
	Three-Dimensional Vertex
	========================
		Stores information about a three dimensional vertex, such as its position,
		normal vector, and texture (UV) coordinates.
	"""
	def __init__(self, x = 0, y = 0, z = 0):
		self.pos = Point3d(x, y, z)
		self.normal = None
		self.texture_coord = None
	
	def __repr__(self):
		return self.pos + ", " + self.normal + ", " + self.texture_coord

#-------------------------------------------------------------------------------
class Polygon:
	"""
	Polygon
	=======
		Stores info about a polygon's vertices and the material to be applied to
		it when rendered.
	"""
	def __init__(self):
		self.vertices = []
		self.material = "Default"

#-------------------------------------------------------------------------------
class Mesh:
	"""
	Mesh Object
	===========
		Stores data about a mesh, such as its vertices, normals, texture coordinates,
		polygons, materials, etc.
		
		A display list is created using the mesh data when first rendered and used
		from that point on to speed rendering.
	"""
	def __init__(self, filename = None):
		self.clear()
		if filename != None:
			self.load(filename)
	
	def clear(self):
		self.vertices = []
		self.normals = []
		self.texture_coords = []
		self.polygons = []
		self.materials = {}
		self.display_list = None
		self.bounding_box = None
	
	def load(self, filename):
		self.clear()
		material = None
		Log.debug("Loading " + filename)
		data = VirtualFS.open(filename).readlines()
		dir, file = os.path.split(filename)
		for line in data:
			if line[:6] == "mtllib":
				self.load_materials(os.path.join(dir, line[7:].strip()))
			elif line[:2] == "vn":
				normals = line[3:].strip().split(" ")
				self.normals.append([float(normals[0]), float(normals[1]), float(normals[2])])
			elif line[:2] == "vt":
				tex = line[3:].strip().split(" ")
				self.texture_coords.append([float(tex[0]), float(tex[1])])
			elif line[0] == "v":
				verts = line[2:].strip().split(" ")
				self.vertices.append([float(verts[0]), float(verts[1]), float(verts[2])])
			elif line[:6] == "usemtl":
				material = line[7:].strip()
			elif line[0] == "f":
				polys = line[2:].strip().split(" ")
				newpoly = Polygon()
				newpoly.material = material
				for p in polys:
					parts = p.split("/")
					if len(parts) == 1:
						newpoly.vertices.append([int(parts[0]) - 1, None, None])
					elif len(parts) == 2:
						newpoly.vertices.append([int(parts[0]) - 1, int(parts[1]) - 1, None])
					elif len(parts) == 3:
						if parts[1] == '':
							newpoly.vertices.append([int(parts[0]) - 1, None, int(parts[2]) - 1])
						else:
							newpoly.vertices.append([int(parts[0]) - 1, int(parts[1]) - 1, int(parts[2]) - 1])
				self.polygons.append(newpoly)
		
		Log.debug("Loaded " + str(len(self.vertices)) + " vertices.")
		Log.debug("Loaded " + str(len(self.polygons)) + " polygons.")

	def load_materials(self, filename):
		Log.debug("Loading " + filename)
		data = VirtualFS.open(filename).readlines()
		current = None
		for line in data:
			if line[:6] == "newmtl":
				if current != None:
					self.materials[current.name] = current
				current = Material(line[7:].strip())
			elif line[:2] == "Ka":
				colors = line[3:].strip().split(" ")
				current.ambient.red = float(colors[0])
				current.ambient.green = float(colors[1])
				current.ambient.blue = float(colors[2])
			elif line[:2] == "Kd":
				colors = line[3:].strip().split(" ")
				current.diffuse.red = float(colors[0])
				current.diffuse.green = float(colors[1])
				current.diffuse.blue = float(colors[2])
			elif line[:2] == "Ks":
				colors = line[3:].strip().split(" ")
				current.specular.red = float(colors[0])
				current.specular.green = float(colors[1])
				current.specular.blue = float(colors[2])
			elif line[0] == "d":
				current.alpha = float(line[3:].strip())
			elif line[:2] == "Ns":
				current.shininess = int(float(line[3:].strip()))
		if current:
			self.materials[current.name] = current
		Log.debug("Loaded " + str(len(self.materials)) + " materials.")
	
	def render(self):
		glPushMatrix()
		if self.display_list != None:
			glCallList(self.display_list)
		else:
			list = glGenLists(1)
			glNewList(list, GL_COMPILE_AND_EXECUTE)
			for poly in self.polygons:
				self.materials[poly.material].set()
				glBegin(GL_POLYGON)
				for vertex, texture, normal in poly.vertices:
					if texture != None:
						glTexCoord2fv(self.texture_coords[texture])
					if normal != None:
						glNormal3fv(self.normals[normal])
					glVertex3fv(self.vertices[vertex])
				glEnd()
			glEndList()
			self.display_list = list
		glPopMatrix()
