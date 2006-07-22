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
#include <map>

#include "gl.h"

using namespace std;

namespace Boom
{
	namespace Graphics
	{
		/*
			Structures to hold a position in two- or three-space.
		*/
		struct Point2d
		{
			float x, y;
		};

		struct Point3d
		{
			float x, y, z;
		};
		
		/*
			Structures to hold mesh material properties
		*/
		struct Color
		{
			float red, green, blue, alpha;
		};
		
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
			
			void set();
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
				
				void clear();
				void load(const char *filename);
				void render();
			
			private:
				int display_list;
				bool textured;
				
				void load_materials(const char *filename);
				void generate_display_list();
		};
		
		struct SceneObject: public Point3d
		{
			string mesh;
			bool render;
		};
		
		class Scene
		{
			public:
				Scene()
				{
					current_id = 0;
				}
				
				~Scene() {}
				
				void clear();
				int add(const char *mesh_name, float x, float y, float z);
				bool move(int id, float dx, float dy, float dz);
				void remove(int id);
				void render();
			
			private:
				unsigned int current_id;
				map <unsigned int, SceneObject> objects;
				map <string, Mesh> meshes;
		};
	}
}

#endif
