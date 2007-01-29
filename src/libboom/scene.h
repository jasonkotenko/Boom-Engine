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
#include "interface.h"

using namespace std;

namespace Boom
{
	/// The scene namespace
	/*!
		Contains functions and objects to create, manipulate, and render objects
		within a three dimensional scene.
	*/
	namespace Scene
	{
		//! The type of object (e.g. a player or an item)
		enum ObjectType
		{
			TYPE_LEVEL,
			TYPE_PLAYER,
			TYPE_BOMB,
			TYPE_ITEM,
			TYPE_CUSTOM_START
		};
		
		//! The collision detection type to use for the object
		enum CollisionType
		{
			COLLISION_NONE,					// No collision detection
			COLLISION_BOUNDING_SPHERE,		// Bounding sphere detection
			COLLISION_CONVEX_HULL,			// Convex hull polygon detection
			COLLISION_EXACT					// Per-polygon detection
		};
		
		//! Generates a unique ID for each object in the scene
		typedef unsigned long ObjectID;
		
		class Scene;
		
		/// An static object within a scene
		/*!
			This represents an static object within a three dimensional scene.
			It consists of an ID, position, mesh, collision info, etc.
		*/
		struct Object: public Graphics::Point3d
		{
			string 			mesh;
			bool 			do_render;
			CollisionType 	collision_type;
			ObjectID		id;
			
			Object();
			Object(const char *mesh, float x, float y, float z);
			virtual ~Object();
			
			//! Update the object (can be overridden to e.g. move an object)
			virtual void update(Scene *scene);
			//! Render the object's mesh to the scene
			virtual void render(Scene *scene);
		};
		/// A movable object within a scene
		/*!
			This represents an object which can be given a speed and angle
			to move within the scene.
		*/
		struct MovableObject: public Object
		{
			double angle;
			int    angle_deg;
			double speed;
			bool   moving;
			
			MovableObject(const char *mesh, float x, float y, float z);
			virtual void update(Scene *scene);
			virtual void render(Scene *scene);
			virtual void update_angle(double angle);
		};
		
		typedef list <Object *> ObjectList;
		typedef map <ObjectType, ObjectList> TypedObjectLists;
		typedef map <string, Graphics::Mesh> MeshList;
		
		/// A three dimensional scene
		/*!
			Represents a three dimensional scene with objects. Manages the
			creation, manipulation, deletion, and rendering of objects.
		*/
		class Scene
		{
			public:
				Scene();
				~Scene();
				
				//! Clear all objects from the scene
				void clear();
				//! Add an object to the scene
				ObjectID add(ObjectType obj, Object *obj);
				//! Remove an object from the scene based on it's ID
				void remove(ObjectID id);
				
				//! Update all objects in the scene
				void update();
				//! Render all objects in the scene
				void render();
			
			//private:
				//! A list of all objects in the scene sorted by type
				TypedObjectLists objects;
				//! An unsorted list of all objects in the scene
				ObjectList		 objects_flat;
				//! A list of loaded meshes in the scene
				MeshList		 meshes;
		};
	}
}

#endif
