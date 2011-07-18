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
#include "camera.h"

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
			TYPE_BLOCK,
			TYPE_ITEM,
			TYPE_CUSTOM
		};
		
		//! The collision detection type to use for the object
		enum CollisionType
		{
			COLLISION_NONE,					// No collision detection
			COLLISION_BOUNDING_SPHERE,		// Bounding sphere detection
			COLLISION_CONVEX_HULL,			// Convex hull polygon detection
			COLLISION_EXACT					// Per-polygon detection
		};
		
		//! A list of item types currently supported
		enum ItemType { FASTER,
						SLOWER,
						EXTRA_BOMB,
						BIGGER_BOMBS,
						SMALLER_BOMBS,
						BOMB_LONG_LIFE,
						BOMB_SHORT_LIFE,
						ITEM_TYPE_COUNT };
		
		//! Generates a unique ID for each object in the scene
		typedef unsigned long ObjectID;
		
		//! Default life of a bomb
		const float DEFAULT_BOMB_LIFE = 5.0;
		
		//! Default size of an explosion
		const float DEFAULT_BOMB_SIZE = 2.0;
		
		//! Default speed of players
		const float DEFAULT_PLAYER_SPEED = 3.0;
		
		class Scene;
		
		/// An static object within a scene
		/*!
			This represents an static object within a three dimensional scene.
			It consists of an ID, position, mesh, collision info, etc.
		*/
		struct Object: public Graphics::Point3d
		{
			string 			mesh;
			string			animation;
			int				frame;
			double			frame_timer;
			bool 			do_render;
			CollisionType 	collision_type;
			ObjectID		id;
			Point3d			rotation;
			Point3d			scale;
			float			life;
			ObjectType		type;
			
			Object(const char *mesh = "\0", float x = 0, float y = 0,
				   float z = 0);
			virtual ~Object();
			
			//! Update the object (can be overridden to e.g. move an object)
			virtual bool update(Scene *scene);
			//! Render the object's mesh to the scene
			virtual void render(Scene *scene);
			//! Called when an object's life (timer) runs out
			virtual bool timeout();
			//! Set an animation
			virtual int set_animation(string name);
		};
		
		/// A movable object within a scene
		/*!
			This represents an object which can be given a speed and angle
			to move within the scene.
		*/
		struct MovableObject: public Object
		{
			double speed;
			bool   moving;
			
			MovableObject(const char *mesh = "\0", float x = 0, float y = 0, 
						  float z = 0);
			virtual bool update(Scene *scene);
		};
		
		/// A simple animated object
		/*!
			This represents an object that can rotate, bounce, throb, etc by
			using transformations and a sin wave.
		*/
		struct SimpleAnimatedObject: public Object
		{
			double rotate_speed;
			bool   rotating;
			
			double bob_speed;
			double bob_mod;
			bool   bobbing;
			
			double throb_speed;
			double throb_mod;
			bool   throbbing;
			
			Point3d move_to;
			double  move_speed;
			bool    move_to_moving;
			
			Point3d rotate_to;
			bool    rotate_to_rotating;
			
			Point3d scale_to;
			double  scale_speed;
			bool    scale_to_scaling;
			
			SimpleAnimatedObject(const char *mesh = "\0", float x = 0,
								 float y = 0, float z = 0);
			virtual bool update(Scene *scene);
			
			protected:
				double sin_pos;
		};
		
		/// A simple bomb
		/*!
			This represents a bomb in the game.
		*/
		struct BombObject: public SimpleAnimatedObject
		{
			BombObject(float x = 0, float y = 0, float z = 0);
			virtual bool update(Scene *scene);
			virtual void render(Scene *scene);
			virtual bool timeout();
			
			float radius;
			
			protected:
				bool  exploding;
				float current_radius;
		};
		
		/// A destructable block
		/*!
			Represents a destructable block in the scene. When destroyed, a
			block will leave a random item behind.
		*/
		struct BlockObject: public Object
		{
			BlockObject(float x = 0, float y = 0, float z = 0);
			virtual void spawn_item(Scene *scene, ObjectID parent);
		};
		
		/// An item that can modify a player
		/*!
			Represents an item that can modify the attributes of a player,
			such as the speed at which the player moves or the size of the
			player's bomb explosions.
		*/
		struct Item: public SimpleAnimatedObject
		{
			Item(float x = 0, float y = 0, float z = 0);
			virtual void apply(Object *player);
			
			ObjectID parent;
			
			protected:
				ItemType item_type;
		};
		
		/// A player
		/*!
			A convenience object for a player that can lay bombs, based on the
			simple movable object above.
		*/
		struct Player: public MovableObject
		{
			Player(float x = 0, float y = 0, float z = 0);
			virtual bool update(Scene *scene);
			
			//! Lay a bomb at the player's current position
			virtual void lay_bomb(Scene *scene);
			
			int   bomb_bag;	//< How many bombs a player can lay at once
			float bomb_size;	//< Size of the bomb's explosion
			float bomb_life;	//< How long until a bomb explodes
			
			protected:
				int bombs_laid;	//< Number of active bombs
		};
		
		//! The current action of an AI player
		enum AIAction { NONE, ESCAPE, CHASE };
		
		/// An AI controlled player
		/*!
			A subclass of the player object to create a simple artificial
			intelligence.
		*/
		struct AIPlayer: public Player
		{
			AIPlayer(float x = 0, float y = 0, float z = 0);
			virtual bool update(Scene *scene);
			
			protected:
				AIAction current_action;	//< None, escaping, chasing, etc
				Object	 *target;			//< Who the action relates to
				float	 decision_timer;	//< When to make new decisions
				
				void select_action(Scene *scene);	//< Decide what to do
		};
		
		typedef list <Object *> ObjectList;
		typedef map <ObjectType, ObjectList> TypedObjectLists;
		typedef map <string, Graphics::BMesh*> MeshList;
		
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
				//! Preload mesh data
				void preload(string mesh);
				//! Add an object to the scene
				ObjectID add(ObjectType type, Object *obj);
				//! Remove an object from the scene based on it's ID
				void remove(ObjectID id);
				
				//! Update all objects in the scene
				void update();
				//! Render all objects in the scene
				void render();
				
				//! The camera through which the scene is rendered
				Camera camera;
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
