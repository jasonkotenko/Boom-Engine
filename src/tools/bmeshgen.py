#!/usr/bin/env python

"""
	Boom Mesh Generator
	===================
	Takes one or more .obj and .mtl files and converts them into the Boom
	engine's animated mesh format for use in games.
	
	Syntax
	------
	bmeshgen.py --anim-walking=walk0.obj,walk1.obj --anim-default=default.obj
"""

import os, os.path
import sys

class Point2d:
	def __init__(self, x = 0, y = 0):
		self.x = x
		self.y = y
	
	def __cmp__(self, other):
		if self.x < other.x:
			return -1
		elif self.x == other.x:
			if self.y < other.y:
				return -1
			elif self.y == other.y:
				return 0
			else:
				return 1
		else:
			return 1
	
	def __repr__(self):
		return str(self.x) + " " + str(self.y)

class Point3d:
	def __init__(self, x = 0, y = 0, z = 0):
		self.x = x
		self.y = y
		self.z = z
	
	def __cmp__(self, other):
		if self.x < other.x:
			return -1
		elif self.x == other.x:
			if self.y < other.y:
				return -1
			elif self.y == other.y:
				if self.z < other.z:
					return -1
				elif self.z == other.z:
					return 0
				else:
					return 1
			else:
				return 1
		else:
			return 1
	
	def __repr__(self):
		return str(self.x) + " " + str(self.y) + " " + str(self.z)

class Color:
	def __init__(self, red = 0, green = 0, blue = 0):
		self.red = red
		self.green = green
		self.blue = blue
	
	def __repr__(self):
		return str(self.red) + " " + str(self.green) + " " + str(self.blue)

class Material:
	def __init__(self, name = "default"):
		self.name = name
		self.ambient = Color()
		self.diffuse = Color(0.5, 0.5, 0.5)
		self.specular = Color(0.5, 0.5, 0.5)
		self.shiny = 96.0
		self.alpha = 1.0
		self.texture = None
	
	def write(self, outfile):
		outfile.write("material " + self.name + "\n")
		outfile.write("ambient " + str(self.ambient) + "\n")
		outfile.write("diffuse " + str(self.diffuse) + "\n")
		outfile.write("specular " + str(self.specular) + "\n")
		outfile.write("shiny " + str(self.shiny) + "\n")
		outfile.write("alpha " + str(self.alpha) + "\n")
		if self.texture:
			outfile.write("texture " + self.texture + "\n")

class PolygonPoint:
	def __init__(self):
		self.vertex = None
		self.normal = None
		self.texture_coord = None

class Polygon:
	def __init__(self, material = "default"):
		self.material = material
		self.points = []

class Mesh:
	def __init__(self, filename = None):
		self.vertices = []
		self.normals = []
		self.texture_coords = []
		self.polygons = []
		self.materials = []
		if filename:
			self.load(filename)
	
	def load(self, filename):
		print "Loading " + filename
	
		data = open(filename).readlines()
		dir, file = os.path.split(filename)
		
		material = "default"
		
		for line in data:
			if line[:6] == "mtllib":
				# Load a material library
				self.load_materials(os.path.join(dir, line[7:].strip()))
			elif line[:2] == "vn":
				# A vertex normal (x, y, z)
				normals = line[3:].strip().split(" ")
				x, y, z = float(normals[0]), float(normals[1]), float(normals[2])
				self.normals.append(Point3d(x, y, z))
			elif line[:2] == "vt":
				# A vertex texture coordinate (x, y)
				tex = line[3:].strip().split(" ")
				self.texture_coords.append(Point2d(float(tex[0]), -float(tex[1])))
			elif line[0] == "v":
				# A vertex (x, y, z)
				verts = line[2:].strip().split(" ")
				x, y, z = float(verts[0]), float(verts[1]), float(verts[2])
				self.vertices.append(Point3d(x, y, z))
			elif line[:6] == "usemtl":
				# Set the current material
				material = line[7:].strip()
			elif line[0] == "f":
				# A face (polygon)
				# Format is number of vertices and then a list of vertex indexes
				polys = line[2:].strip().split(" ")
				newpoly = Polygon(material)
				for p in polys:
					parts = p.split("/")
					
					point = PolygonPoint()
					point.vertex = self.vertices[int(parts[0]) - 1]
					
					if len(parts) == 1:
						# Just a vertex
						newpoly.points.append(point)
					elif len(parts) == 2:
						# A vertex and normal
						point.normal = self.normals[int(parts[1]) - 1]
						newpoly.points.append(point)
					elif len(parts) == 3:
						# A vertex, normal, and texture coordinate, though the 
						# texture coordinate could be blank!
						point.normal = self.normals[int(parts[2]) - 1]
						if parts[1] != '':
							point.texture_coord = self.texture_coords[int(parts[1]) - 1]
						newpoly.points.append(point)
				self.polygons.append(newpoly)
		
	def load_materials(self, filename):
		data = open(filename).readlines()
		current = None
		for line in data:
			if line[:6] == "newmtl":
				# Start a new material
				if current != None:
					# Save the last one
					self.materials.append(current)
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
				current.shiny = int(float(line[3:].strip()))
			elif line[:6] == "map_Kd":
				# Create a new texture
				current.texture = line[7:].strip()
		
		if current:
			# Save the last material
			self.materials.append(current)

class FramePoly:
	def __init__(self, material = "default"):
		self.points = []
		self.material = material
	
	def write(self, outfile):
		outfile.write("poly " + self.material + "\n")
		for v, vn, vt in self.points:
			outfile.write("point " + str(v) + " " + str(vn) + " " + str(vt) + "\n")

class Frame:
	def __init__(self, material = "default"):
		self.polys = []
	
	def write(self, outfile):
		outfile.write("frame\n")
		for poly in self.polys:
			poly.write(outfile)

class Animation:
	def __init__(self, name = "default", speed = 1.0):
		self.name = name
		self.speed = speed
		self.frames = []
	
	def write(self, outfile):
		outfile.write("animation " + self.name + "\n")
		outfile.write("speed " + str(self.speed) + "\n")
		for frame in self.frames:
			frame.write(outfile)

class BMesh:
	def __init__(self):
		self.vertices = []
		self.normals = []
		self.texture_coords = []
		self.animations = []
		self.materials = []
	
	def stats(self):
		frames = 0
		for animation in self.animations:
			frames += len(animation.frames)
		
		print "Vertices: " + str(len(self.vertices))
		print "Normals: " + str(len(self.normals))
		print "Texture Coordinates: " + str(len(self.texture_coords))
		print "Materials: " + str(len(self.materials))
		print "Animations: " + str(len(self.animations))
		print "Total Frames: " + str(frames)
	
	def write(self, name):
		print "Writing materials to " + name + ".bmat"
		self.writemat(name)
		print "Writing mesh to " + name + ".bmesh"
		self.writemesh(name)
	
	def writemat(self, name):
		outfile = open(name + ".bmat", "w")
		for material in self.materials:
			material.write(outfile)
			outfile.write("\n")
	
	def writemesh(self, name):
		outfile = open(name + ".bmesh", "w")
		outfile.write("materials " + name + ".bmat\n\n")
		for tag, list in [["v", self.vertices],
						  ["vn", self.normals],
						  ["vt", self.texture_coords]]:
			for item in list:
				outfile.write(tag + " " + str(item) + "\n")
			if not len(list):
				if tag == "vt":
					outfile.write(tag + " 0 0\n")
				else:
					outfile.write(tag + " 0 0 0\n")
		
		for animation in self.animations:
			outfile.write("\n")
			animation.write(outfile)

def find(list, what):
	for index, item in enumerate(list):
		if item == what:
			return index
	return None

def add_mesh_data(inmesh, outmesh):
	for inlist, outlist in [[inmesh.vertices, outmesh.vertices],
							[inmesh.normals, outmesh.normals],
							[inmesh.texture_coords, outmesh.texture_coords]]:
		for point in inlist:
			loc = find(outlist, point)
			if loc == None:
				outlist.append(point)
				point.link = len(outlist) - 1
			else:
				point.link = loc
				
	for material in inmesh.materials:
		found = False
		for material2 in outmesh.materials:
			if material.name == material2.name:
				found = True
		if not found:
			outmesh.materials.append(material)

def add_mesh_animation(mesh, name, frames):
	anim = Animation(name)
	mesh.animations.append(anim)
	for frame in frames:
		bframe = Frame()
		for poly in frame.polygons:
			current = FramePoly(poly.material)
			for point in poly.points:
				if not point.normal:
					normal = 0
				else:
					normal = point.normal.link
				if not point.texture_coord:
					texture_coord = 0
				else:
					texture_coord = point.texture_coord.link
				current.points.append([point.vertex.link, normal, texture_coord])
			bframe.polys.append(current)
		anim.frames.append(bframe)

#==============================================================================

name = "test"
animations = {}
outmesh = BMesh()

def print_help():
	print "Boom Mesh Generator, usage:"
	print sys.argv[0] + " --anim-walk=walk0.obj,walk1.obj --anim-default=test.obj newname"
	sys.exit(1)

if len(sys.argv) < 2:
	print_help()

for arg in sys.argv[1:]:
	if arg == "--help" or arg == "-h":
		print_help()
	elif arg[:6] == "--anim":
		parts = arg.split("=")
		animation = parts[0][7:]
		frames = []
		for filename in parts[1].split(","):
			frames.append(Mesh(filename))
			add_mesh_data(frames[-1], outmesh)
		animations[animation] = frames
	else:
		name = arg

for key in animations:
	add_mesh_animation(outmesh, key, animations[key])
		
outmesh.stats()
outmesh.write(name)
