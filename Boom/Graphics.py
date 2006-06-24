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
from math import pow, sqrt

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
	Log.info("Please install them from http://pyopengl.sourceforge.net/")
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
	
	def array(self):
		return [self.x, self.y]
	
	def __repr__(self):
		"""
		Represent the point as [x, y]
		"""
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
	
	def array(self):
		return [self.x, self.y, self.z]
	
	def __repr__(self):
		"""
		Represent the point as [x, y, z]
		"""
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
		"""
		Return an array of [red, green, blue, alpha]
		"""
		return [self.red, self.green, self.blue, self.alpha]
	
	def ___repr___(self):
		"""
		Represent the color as [red, green, blue, alpha]
		"""
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
		"""
		Load the image from a file.
		"""
		Log.debug("Loading " + filename)
		# Clear previous data
		self.width = 0
		self.height = 0
		self.data = []
		# Check the format of the file
		if filename[-4:] == ".ppm":
			# Load from a PPM
			return self.load_ppm(VirtualFS.open(filename))
		else:
			Log.error("Uknown image format!")
	
	def load_ppm(self, data):
		"""
		Load an image from a Portable PixMap file. Warning: this is SLOW!
		"""
		data2 = []
		# Strip out all comment lines
		for line in data:
			if line[0] != "#":
				for part in line.split("\t"):
					for piece in part.split(" "):
						if piece and ord(piece[0]) >= 48:
							data2.append(piece.strip())
		# Check for magic number
		if data2[0] != "P3":
			Log.warning("PPM Header not found...")
			return False
		
		# Set the width and height of the image
		self.width = int(data2[1])
		self.height = int(data2[2])
		
		# Setup the data array
		for y in range(self.height):
			self.data.append([])
		
		# Get the maximum allowed color value per component (e.g. 255)
		maxvalue = int(data2[3])
		pos = 4
		
		# Read in the image data
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
		"""
		Set this material as the current OpenGL material for rendering.
		"""
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
		"""
		Represent the vertex as position, normal, texture coordinate
		"""
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
		"""
		Represent the vertex as position, normal, texture coordinate
		"""
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
		"""
		Clear all data in the mesh, including vertices, normals, polygons, materials,
		texture coordinates, etc.
		"""
		self.vertices = []
		self.normals = []
		self.texture_coords = []
		self.polygons = []
		self.materials = {}
		self.display_list = None
		#self.bounding_box = None
		self.center = Point3d()
		self.radius = 0
	
	def load(self, filename):
		"""
		Load the mesh from an OBJ file.
		"""
		# Clear old data
		self.clear()
		
		# Default material to none until one is set
		material = None
		
		Log.debug("Loading " + filename)
		data = VirtualFS.open(filename).readlines()
		dir, file = os.path.split(filename)
		
		#bbmin = Point3d(1000, 1000, 1000)
		#bbmax = Point3d(-1000, -1000, -1000)
		
		# Process the file line by line
		for line in data:
			if line[:6] == "mtllib":
				# Load a material library
				self.load_materials(os.path.join(dir, line[7:].strip()))
			elif line[:2] == "vn":
				# A vertex normal (x, y, z)
				normals = line[3:].strip().split(" ")
				self.normals.append([float(normals[0]), float(normals[1]), float(normals[2])])
			elif line[:2] == "vt":
				# A vertex texture coordinate (x, y)
				tex = line[3:].strip().split(" ")
				self.texture_coords.append([float(tex[0]), float(tex[1])])
			elif line[0] == "v":
				# A vertex (x, y, z)
				verts = line[2:].strip().split(" ")
				x, y, z = float(verts[0]), float(verts[1]), float(verts[2])
				"""
				if x < bbmin.x:
					bbmin.x = x
				if x > bbmax.x:
					bbmax.x = x
				if y < bbmin.y:
					bbmin.y = y
				if y > bbmax.y:
					bbmax.y = y
				if z < bbmin.z:
					bbmin.z = z
				if z > bbmax.z:
					bbmax.z = z
				"""
				self.vertices.append(Vertex3d(x, y, z))
			elif line[:6] == "usemtl":
				# Set the current material
				material = line[7:].strip()
			elif line[0] == "f":
				# A face (polygon)
				# Format is number of vertices and then a list of vertex indexes
				polys = line[2:].strip().split(" ")
				newpoly = Polygon()
				newpoly.material = material
				for p in polys:
					parts = p.split("/")
					if len(parts) == 1:
						# Just a vertex
						newpoly.vertices.append([int(parts[0]) - 1, None, None])
					elif len(parts) == 2:
						# A vertex and normal
						newpoly.vertices.append([int(parts[0]) - 1, int(parts[1]) - 1, None])
					elif len(parts) == 3:
						# A vertex, normal, and texture coordinate, though the texture coordinate
						# could be blank!
						if parts[1] == '':
							newpoly.vertices.append([int(parts[0]) - 1, None, int(parts[2]) - 1])
						else:
							newpoly.vertices.append([int(parts[0]) - 1, int(parts[1]) - 1, int(parts[2]) - 1])
				self.polygons.append(newpoly)
				
		#self.bounding_box = [bbmin, bbmax]
		
		Log.debug("Loaded " + str(len(self.vertices)) + " vertices.")
		Log.debug("Loaded " + str(len(self.polygons)) + " polygons.")
		#Log.debug("Bounding box at " + str(bbmin) + ", " + str(bbmax))
		
		for vertex in self.vertices:
			self.center.x += vertex.pos.x
			self.center.y += vertex.pos.y
			self.center.z += vertex.pos.z
		self.center.x /= len(self.vertices)
		self.center.y /= len(self.vertices)
		self.center.z /= len(self.vertices)
		
		Log.info("Object center found at " + str(self.center))
		
		max_dist = 0
		for vertex in self.vertices:
			dist = pow(self.center.x - vertex.pos.x, 2) + \
				   pow(self.center.y - vertex.pos.y, 2) + \
				   pow(self.center.z - vertex.pos.z, 2)
			if dist > max_dist:
				max_dist = dist
		self.radius = sqrt(max_dist)
		
		Log.info("Object radius is " + str(self.radius))

	def load_materials(self, filename):
		"""
		Load a material library from an OBJ compatible MTL file.
		"""
		Log.debug("Loading " + filename)
		data = VirtualFS.open(filename).readlines()
		current = None
		for line in data:
			if line[:6] == "newmtl":
				# Start a new material
				if current != None:
					# Save the last one
					self.materials[current.name] = current
				current = Material(line[7:].strip())
			elif line[:2] == "Ka":
				# Set the ambient properties
				colors = line[3:].strip().split(" ")
				current.ambient.red = float(colors[0])
				current.ambient.green = float(colors[1])
				current.ambient.blue = float(colors[2])
			elif line[:2] == "Kd":
				# Set the diffuse properties
				colors = line[3:].strip().split(" ")
				current.diffuse.red = float(colors[0])
				current.diffuse.green = float(colors[1])
				current.diffuse.blue = float(colors[2])
			elif line[:2] == "Ks":
				# Set the specular properties
				colors = line[3:].strip().split(" ")
				current.specular.red = float(colors[0])
				current.specular.green = float(colors[1])
				current.specular.blue = float(colors[2])
			elif line[0] == "d":
				# Set the translucency (alpha) value
				current.alpha = float(line[3:].strip())
			elif line[:2] == "Ns":
				# Set the material shininess
				current.shininess = int(float(line[3:].strip()))
		if current:
			# Save the last material
			self.materials[current.name] = current
		Log.debug("Loaded " + str(len(self.materials)) + " materials.")
	
	def render(self):
		"""
		Render this mesh to the screen at the current position.
		"""
		# If a display list exists, just call it
		if self.display_list != None:
			glCallList(self.display_list)
		else:
			# Generate a new display list and render
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
					glVertex3fv(self.vertices[vertex].pos.array())
				glEnd()
				"""
				# Draw the mesh's bounding sphere
				glPushMatrix()
				glTranslatef(self.center.x, self.center.y, self.center.z)
				glutWireSphere(self.radius, 12, 12)
				glPopMatrix()
				"""
			glEndList()
			self.display_list = list
