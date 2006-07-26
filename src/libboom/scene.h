/*
	Boom Scene Manager
	==================
		Scene objects and functions.
		
		Copyright 2006 Daniel G. Taylor
*/

#ifndef _SCENE_H
#define _SCENE_H

#include <list>
#include <map>

#include "graphics.h"

using namespace std;

namespace Boom
{
	namespace Scene
	{
		enum ObjectType
		{
			TYPE_LEVEL,
			TYPE_PLAYER,
			TYPE_BOMB,
			TYPE_ITEM,
			TYPE_CUSTOM_START
		};
		
		enum CollisionType
		{
			COLLISION_NONE,					// No collision detection
			COLLISION_BOUNDING_SPHERE,		// Bounding sphere detection
			COLLISION_CONVEX_HULL,			// Convex hull polygon detection
			COLLISION_EXACT					// Per-polygon detection
		};
		
		typedef unsigned long ObjectID;
		
		class Scene;
		
		struct Object: public Graphics::Point3d
		{
			string 			mesh;
			bool 			do_render;
			CollisionType 	collision_type;
			ObjectID		id;
			
			Object();
			Object(const char *mesh, float x, float y, float z);
			virtual ~Object();
			
			virtual void update(Scene *scene);
			virtual void render(Scene *scene);
		};
		
		typedef list <Object *> ObjectList;
		typedef map <ObjectType, ObjectList> TypedObjectLists;
		typedef map <string, Graphics::Mesh> MeshList;
		
		class Scene
		{
			public:
				Scene();
				~Scene();
				
				void clear();
				ObjectID add(ObjectType obj, Object *obj);
				void remove(ObjectID id);
				
				void update();
				void render();
			
			private:
				TypedObjectLists objects;
				ObjectList		 objects_flat;
				MeshList		 meshes;
		};
	}
}

#endif
