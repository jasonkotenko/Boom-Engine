#!/usr/bin/env python

"""
	Boom Graphics Objects
	=====================
		This module defines classes for graphical objects such as points,
		images, materials, meshes, as well as providing access to OpenGL
		to the rest of the engine.
	
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
from math import sin, cos, sqrt, atan2, pi
from copy import deepcopy

import Log
Log.info("Initializing graphics subsystem...")

import VirtualFS

# OpenGL functions
try:
	from OpenGL.GL import *
	from OpenGL.GLU import *
	from OpenGL.GLUT import *
except:
	Log.critical("Can't import OpenGL bindings...")
	Log.info("Please install them from http://pyopengl.sourceforge.net/")
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
class Vertex2d(Point2d):
	"""
	Two-Dimensional Vertex
	======================
		Stores information about a two dimensional vertex, such as its position,
		normal vector, and texture (UV) coordinates.
	"""
	def __init__(self, x = 0, y = 0):
		Point2d.__init__(self, x, y)
		self.normal = None
		self.texture_coord = None
	
	def __repr__(self):
		"""
		Represent the vertex as position, normal, texture coordinate
		"""
		return Point2d.__repr__(self) + ", " + str(self.normal) + ", " + str(self.texture_coord)

#-------------------------------------------------------------------------------
class Vertex3d(Point3d):
	"""
	Three-Dimensional Vertex
	========================
		Stores information about a three dimensional vertex, such as its position,
		normal vector, and texture (UV) coordinates.
	"""
	def __init__(self, x = 0, y = 0, z = 0):
		Point3d.__init__(self, x, y, z)
		self.normal = None
		self.texture_coord = None
	
	def __repr__(self):
		"""
		Represent the vertex as position, normal, texture coordinate
		"""
		return Point3d.__repr__(self) + ", " + str(self.normal) + ", " + str(self.texture_coord)

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
		
		self.generate_convex_hull()
	
	def generate_convex_hull(self):
		self.hull = convex_hull2d(self.vertices)
		Log.debug("Hull generated with " + str(len(self.hull)) + " points")
		
		self.hull = optimize_hull2d(self.hull, 6)
		Log.debug("Hull optimized to " + str(len(self.hull)) + " points")

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
			dlist = glGenLists(1)
			glNewList(dlist, GL_COMPILE_AND_EXECUTE)
			for poly in self.polygons:
				self.materials[poly.material].set()
				glBegin(GL_POLYGON)
				for vertex, texture, normal in poly.vertices:
					if texture != None:
						glTexCoord2fv(self.texture_coords[texture])
					if normal != None:
						glNormal3fv(self.normals[normal])
					glVertex3fv(self.vertices[vertex].array())
				glEnd()
				"""
				# Draw the mesh's bounding sphere
				glPushMatrix()
				glTranslatef(self.center.x, self.center.y, self.center.z)
				glutWireSphere(self.radius, 12, 12)
				glPopMatrix()
				"""
				# Draw convex hull
				glPushMatrix()
				glDisable(GL_LIGHTING)
				glColor3f(0, 1.0, 0)
				glBegin(GL_LINE_LOOP)
				for v in self.hull:
					glVertex3f(v.x, v.y, 0.1)
				glEnd()
				glEnable(GL_LIGHTING)
				glPopMatrix()
			glEndList()
			self.display_list = dlist

#-------------------------------------------------------------------------------
class Hull2d(list):
	def __init__(self):
		list.__init__(self)
		self.__center = None
		self.__radius = None
	
	def __setitem__(self, item, value):
		self.__center = None
		self.__radius = None
		list.__setitem__(self, item, value)
	
	def __get_center(self):
		if self.__center != None:
			return self.__center
		else:
			self.__center = Point2d(0, 0)
			for vertex in self:
				self.__center.x += vertex.x
				self.__center.y += vertex.y
			self.__center.x /= len(self)
			self.__center.y /= len(self)
			return self.__center
	
	def __set_center(self, center):
		self.__center = center
	
	def __get_radius(self):
		if self.__radius != None:
			return self.__radius
		else:
			if self.__center == None:
				center = self.__get_center()
			else:
				center = self.__center
			max_dist = 0
			for vertex in self:
				dx = center.x - vertex.x
				dy = center.y - vertex.y
				dist = dx * dx + dy * dy
				if dist > max_dist:
					max_dist = dist
			self.__radius = sqrt(max_dist)
			return self.__radius
	
	def __set_radius(self, radius):
		self.__radius = radius
	
	center = property(__get_center, __set_center)
	radius = property(__get_radius, __set_radius)

"""
def line_intersection2d(p1, p2, offset1, p3, p4, offset2):
	try:
		m1 = (p2.y - p1.y) / (p2.x - p1.x)
		m2 = (p4.y - p3.y) / (p4.x - p3.x)
		#print "Slopes", m1, m2
	except:
		return False
	
	b1 = (p1.y + offset1.y) - (m1 * (p1.x + offset1.x))
	b2 = (p3.y + offset2.y) - (m2 * (p3.x + offset2.x))
	#print "Intercepts", b1, b2
	
	if m1 == m2:
		return False # Parallel
	
	x = (b2 - b1) / (m1 - m2)
	y = (m1 * x) + b1
	
	#print x, y
	
	if p1.x < p2.x:
		t1 = p1
		t2 = p2
	else:
		t1 = p2
		t2 = p1
	if p3.x < p4.x:
		t3 = p3
		t4 = p4
	else:
		t3 = p4
		t4 = p3
		
	#print t1, t2, t3, t4
	
	if (x >= t1.x + offset1.x and x <= t2.x + offset1.x and x >= t3.x + offset2.x and x <= t4.x + offset2.x):
		return True
	else:
		return False
"""

#-------------------------------------------------------------------------------
def line_intersection2d(p1, p2, offset1, p3, p4, offset2):
	# Calculate offset points
	x1 = p1.x + offset1.x
	y1 = p1.y + offset1.y
	x2 = p2.x + offset1.x
	y2 = p2.y + offset1.y
	x3 = p3.x + offset2.x
	y3 = p3.y + offset2.y
	x4 = p4.x + offset2.x
	y4 = p4.y + offset2.y
	
	# Calculate Ax + By = C for the first line
	A1 = y2 - y1
	B1 = x1 - x2
	C1 = A1 * x1 + B1 * y1
	
	# Calculate Ax + By = C for the second line
	A2 = y4 - y3
	B2 = x3 - x4
	C2 = A2 * x3 + B2 * y3
	
	# Calculate the determinant
	det = A1 * B2 - A2 * B1
	
	if det == 0:
		# The lines are parallel
		if A1 == A2 and B1 == B2 and C1 == C2:
			# The line segments lie on the same line, so now check if one point on the
			# second line lies within the two points on the first line
			if x1 < x2:
				return (x3 >= x1 and x3 <= x2) or (x4 >= x1 and x4 <= x2)
			else:
				return (x3 >= x2 and x3 <= x1) or (x4 >= x2 and x4 <= x1)
		else:
			return False
	else:
		# Get the point of intersection
		x = (B2 * C1 - B1 * C2) / det
		y = (A1 * C2 - A2 * C1) / det
		# Check to see if the point of intersection lies on both lines
		if x1 < x2:
			if x3 < x4:
				return x >= x1 and x <= x2 and x >= x3 and x <= x4
			else:
				return x >= x1 and x <= x2 and x >= x4 and x <= x3
		else:
			if x3 < x4:
				return x >= x2 and x <= x1 and x >= x3 and x <= x4
			else:
				return x >= x2 and x <= x1 and x >= x4 and x <= x3

#-------------------------------------------------------------------------------
def hull_collision2d(hull1, offset1, hull2, offset2):
	# Setup point lists
	dist1 = []
	dist2 = []
	
	# Store center and radius for convenience and to keep from calling the
	# Hull2d's property functions repeatedly
	center1 = hull1.center
	center2 = hull2.center
	radius1 = hull1.radius * hull1.radius
	radius2 = hull2.radius * hull2.radius
	
	# Find all points in hull1 that lie within the radius of hull2
	for pos in range(len(hull1)):
		vertex = hull1[pos]
		dx = (vertex.x + offset1.x) - (center2.x + offset2.x)
		dy = (vertex.y + offset1.y) - (center2.y + offset2.y)
		d = dx * dx + dy * dy
		if d <= radius2:
			dist1.append(pos)
	# Add the point before the first and after the last so we use them when
	# making lines to test
	if len(dist1) == 0:
		return False
	elif len(dist1) < len(hull1):
		dist1 = [dist1[0] - 1] + dist1 + [(dist1[-1] + 1) % len(dist1)]
	
	# Find all points in hull2 that lie within the radius of hull1
	for pos in range(len(hull2)):
		vertex = hull2[pos]
		dx = (vertex.x + offset2.x) - (center1.x + offset1.x)
		dy = (vertex.y + offset2.y) - (center1.y + offset1.y)
		d = dx * dx + dy * dy
		if d <= radius1:
			dist2.append(pos)
	# Add the point before the first and after the last so we use them when
	# making lines to test
	if len(dist2) == 0:
		return False
	elif len(dist2) < len(hull2):
		dist2 = [dist2[0] - 1] + dist2 + [(dist2[-1] + 1) % len(dist2)]
	
	# Iterate through points and test lines
	for pos in range(len(dist1) - 1):
		v1 = hull1[dist1[pos]]
		v2 = hull1[dist1[pos + 1]]
		for pos2 in range(len(dist2) - 1):
			v3 = hull2[dist2[pos2]]
			v4 = hull2[dist2[pos2 + 1]]
			if line_intersection2d(v1, v2, offset1, v3, v4, offset2):
				return True
	return False
	
	"""
	for pos in range(len(hull1)):
		v1 = hull1[pos]
		if pos == len(hull1) - 1:
			v2 = hull1[0]
		else:
			v2 = hull1[pos + 1]
		for pos2 in range(len(hull2)):
			v3 = hull2[pos]
			if pos2 == len(hull2) - 1:
				v4 = hull2[0]
			else:
				v4 = hull2[pos2 + 1]
			if line_intersection2d(v1, v2, offset1, v3, v4, offset2):
				return True
	return False
	"""

#-------------------------------------------------------------------------------
def polar_angle2d(pole, point):
	"""
	Returns the polar angle between the pole and a point in radians.
	Output is 0 to 2pi
	"""
	dx = point.x - pole.x
	dy = point.y - pole.y
	angle = atan2(dy, dx)
	return angle % (2 * pi)

#-------------------------------------------------------------------------------
def cross2d(v1, v2, v3):
	return ((v2.x - v1.x) * (v3.y - v1.y)) - ((v3.x - v1.x) * (v2.y - v1.y))

#-------------------------------------------------------------------------------
def distance2d(v1, v2):
	"""
	Returns the distance between two points
	"""
	dx = v1.x - v2.x
	dy = v1.y - v2.y
	return sqrt(dx * dx + dy * dy)

#-------------------------------------------------------------------------------
def optimize_hull2d(hull, vertex_count):
	if len(hull) <= vertex_count:
		return hull
	step = (2 * pi) / vertex_count
	current_angle = 0.0
	new_hull = Hull2d()
	center = hull.center
	radius = hull.radius
	for c in range(vertex_count):
		n = Point2d()
		n.x = center.x + (radius * cos(current_angle))
		n.y = center.y + (radius * sin(current_angle))
		min_d = 1000
		min_p = None
		for vertex in hull:
			d = distance2d(vertex, n)
			if d < min_d:
				min_d = d
				min_p = vertex
		new_hull.append(min_p)
		current_angle += step
	return new_hull

#-------------------------------------------------------------------------------
def convex_hull2d(vertices):
	v = []
	
	# Copy the vertex array
	for vertex in vertices:
		v.append(Point2d(vertex.x, vertex.y))
	
	# Find the bottom-left most point
	pivot = v[0]
	for pos in range(len(v)):
		vertex = v[pos]
		if vertex.y < pivot.y:
			pivot = vertex
		elif vertex.y == pivot.y and vertex.x < pivot.x:
			pivot = vertex
	
	# Save the angles from the pivot
	for vertex in v:
		vertex.angle = polar_angle2d(pivot, vertex)
	
	def compare(v1, v2):
		if v1.angle < v2.angle:
			return -1
		elif v1.angle > v2.angle:
			return 1
		elif v1.x == v2.x and v1.y == v2.y:
			return 1
		elif distance2d(pivot, v1) < distance2d(pivot, v2):
			return -1
		else:
			return 1
	
	# Sort the list by angle, then distance if angles tie
	v.sort(compare)
	
	# Setup the stack
	stack = Hull2d()
	stack.append(v[0])
	stack.append(v[1])
	
	# Build the convex hull out of only left turns
	for pos in range(2, len(v)):
		c = cross2d(stack[-2], stack[-1], v[pos])
		if c == 0:
			stack.pop()
			stack.append(v[pos])
		elif c > 0:
			stack.append(v[pos])
		else:
			while c <= 0 and len(stack) > 2:
				stack.pop()
				c = cross2d(stack[-2], stack[-1], v[pos])
			stack.append(v[pos])
	
	return stack
