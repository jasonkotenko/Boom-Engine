/*
	Boom Graphics
	=============
		Graphics objects and functions.
		
		Copyright 2006 Daniel G. Taylor
*/

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <string>
#include <vector>
#include <list>
#include <map>

#include "gl.h"

using namespace std;

namespace Boom
{
	/// Graphics functions and objects
	/*!
		Functions to do math intensive graphics processing, hull calculation, 
		etc. as well as objects to store points, materials, meshes, etc.
	*/
	namespace Graphics
	{
		/*
			Structures to hold a point in two- or three-space.
		*/
		/// A 2d point
		/*!
			A point in two dimensional space.
		*/
		struct Point2d
		{
			float x, y;
		};

		/// A 3d point
		/*!
			A point in three dimensional space.
		*/
		struct Point3d
		{
			float x, y, z;
		};
		
		/*
			Structures to hold mesh material properties.
		*/
		/// A color
		/*!
			A color, specified by its red, green, blue, and alpha values.
		*/
		struct Color
		{
			float red, green, blue, alpha;
		};
		
		/// A mesh material
		/*!
			A material used to render a mesh. These values mainly correspond to
			the OpenGL material properties.
		*/
		struct Material
		{
			string name;
			Color ambient;
			Color diffuse;
			Color specular;
			float alpha;
			float illumination;
			int shininess;
			GLuint texture;
			bool textured;
			bool texture_alpha;
			
			//! Set this material as active for drawing
			void set();
		};
		
		/*
			Convex hull stuff for collision detection
		*/
		/// A 2d convex hull point
		/*!
			A two dimensional point on a hull that contains the angle and pivot.
			This is used when calculating the convex hull for a mesh.
		*/
		struct HullVertex2d: Point2d
		{
			double	angle;
			Point2d *pivot;
		};
		
		/// A 2d convex hull
		/*!
			A two dimensional convex hull that represents an object. This is 
			currently used for collision detection.
		*/
		struct Hull2d
		{
			//! Constructor
			Hull2d();
			
			//! Generate a convex hull from a mesh's vertices
			void generate(vector <Point3d> &vertices);
			//! Calculate the center point of the hull
			void calc_center();
			//! Calculate the radius of the hull from the center point
			void calc_radius();
			//! Optimize the hull to a number of lines
			void optimize(unsigned int size);
			
			Point2d				center;
			float				radius;
			vector <Point2d>	vertices;
		};

		/*
			
		*/
		struct MeshVertex
		{
			int pos, normal, texture_coord;
		};

		struct MeshPolygon
		{
			vector <MeshVertex> vertices;
			Material *material;
		};
		
		class Mesh
		{
			public:
				vector <Point3d> vertices;
				vector <Point3d> normals;
				vector <Point2d> texture_coords;
				vector <MeshPolygon> polygons;
				map <string, Material> materials;
				Hull2d hull;
				
				void clear();
				void load(const char *filename);
				void render();
			
			private:
				int display_list;
				bool textured;
				
				void load_materials(const char *filename);
				void generate_display_list();
		};
		
		/*
			
		*/
		double distance2d(Point2d *v1, Point2d *v2);
		double polar_angle2d(Point2d *pole, Point2d *point);
		double cross2d(Point2d *v1, Point2d *v2, Point2d *v3);
	}
}

#endif
