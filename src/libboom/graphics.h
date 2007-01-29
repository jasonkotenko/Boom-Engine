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
			
			Point2d				center;		//!< The center point of the hull
			float				radius;		//!< The radius of the hull
			vector <Point2d>	vertices;	//!< A list of hull vertices
		};

		/// A vertex in a mesh
		/*!
			A mesh vertex containing a indices to the vertex, normal, and 
			texture coordinate data.
		*/
		struct MeshVertex
		{
			int pos, normal, texture_coord;
		};
		
		/// A polygon in a mesh
		/*!
			A polygon with a list of vertices and a texture.
		*/
		struct MeshPolygon
		{
			vector <MeshVertex> vertices;
			Material *material;
		};
		
		/// A mesh object
		/*!
			A representation of a mesh consisting of vertices, normals, texture
			coordinates, polygons, and materials.
		*/
		class Mesh
		{
			public:
				vector <Point3d> vertices;
				vector <Point3d> normals;
				vector <Point2d> texture_coords;
				vector <MeshPolygon> polygons;
				map <string, Material> materials;
				Hull2d hull;
				
				//! Clear all the mesh's data
				void clear();
				//! Load the mesh from a file
				void load(const char *filename);
				//! Render the mesh at the current position in the scene
				void render();
			
			private:
				int display_list;
				bool textured;
				
				//! Load the materials from a file
				void load_materials(const char *filename);
				//! Generate a display list for faster rendering
				void generate_display_list();
		};
		
		/*
			Math functions...
		*/
		//! Calculate the distance between two coplanar points
		double distance2d(Point2d *v1, Point2d *v2);
		//! Calculate the polar angle between a pole and a point
		double polar_angle2d(Point2d *pole, Point2d *point);
		//! A coplanar vector cross
		double cross2d(Point2d *v1, Point2d *v2, Point2d *v3);
	}
}

#endif
