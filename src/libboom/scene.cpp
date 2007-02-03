/*
	Scene implementation
*/

#include "log.h"
#include "scene.h"
#include <cmath>

using namespace std;

namespace Boom
{
	namespace Scene
	{
		static ObjectID gid = 0;
		
		//----------------------------------------------------------------------
		Object::Object()
		{
			id = gid++;
			x = y = z = 0;
			do_render = false;
			collision_type = COLLISION_CONVEX_HULL;
			rotation.x = rotation.y = rotation.z = 0;
			scale.x = scale.y = scale.z = 1.0;
		}
		
		//----------------------------------------------------------------------
		Object::Object(const char *mesh, float x, float y, float z)
		{
			id = gid++;
			this->mesh = mesh;
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
			collision_type = COLLISION_CONVEX_HULL;
			rotation.x = rotation.y = rotation.z = 0;
			scale.x = scale.y = scale.z = 1.0;
		}
		
		//----------------------------------------------------------------------
		Object::~Object()
		{
		
		}
		
		//----------------------------------------------------------------------
		void Object::update(Scene *scene)
		{
			
		}
		
		//----------------------------------------------------------------------
		void Object::render(Scene *scene)
		{
			//LOG_INFO << "Rendering object " << id << endl;
			glPushMatrix();
			glTranslatef(x, y, z);
			if (scale.x != 1.0 || scale.y != 1.0 || scale.z != 1.0)
				glScalef(scale.x, scale.y, scale.z);
			if (rotation.x)
				glRotatef(rotation.x, 1.0, 0.0, 0.0);
			if (rotation.y)
				glRotatef(rotation.y, 0.0, 1.0, 0.0);
			if (rotation.z)
				glRotatef(rotation.z, 0.0, 0.0, 1.0);
			scene->meshes[mesh].render();
			glPopMatrix();
		}
		
		//----------------------------------------------------------------------
		MovableObject::MovableObject(const char *mesh, float x, float y, float z)
		{
			Object::Object(mesh, x, y, z);
			this->mesh = mesh;
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
			speed = 0;
			moving = false;
		}
		
		//----------------------------------------------------------------------
		void MovableObject::update(Scene *scene)
		{
			if (moving)
			{
				x += cos((rotation.z - 90) * M_PI / 180.0) * speed * tdiff;
				y += sin((rotation.z - 90) * M_PI / 180.0) * speed * tdiff;
			}
		}
		
		//----------------------------------------------------------------------
		RotatingObject::RotatingObject(const char *mesh, float x, float y, 
									   float z)
		{
			Object::Object(mesh, x, y, z);
			this->mesh = mesh;
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
			rot_speed = 1.0;
			rotating = true;
		}
		
		//----------------------------------------------------------------------
		void RotatingObject::update(Scene *scene)
		{
			if (rotating)
			{
				rotation.z += rot_speed * 30 * tdiff;
				if (rotation.z > 360)
					rotation.z -= 360;
			}
		}
		
		//----------------------------------------------------------------------
		BobbingObject::BobbingObject()
		{
			Object::Object();
			bob_speed = 3.0;
			bob_mod = 0.1;
			bobbing = true;
			bob_pos = 0;
		}
		
		//----------------------------------------------------------------------
		BobbingObject::BobbingObject(const char *mesh, float x, float y, 
									 float z)
		{
			Object::Object(mesh, x, y, z);
			this->mesh = mesh;
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
			bob_speed = 3.0;
			bob_mod = 0.1;
			bobbing = true;
			bob_pos = 0;
		}
		
		//----------------------------------------------------------------------
		void BobbingObject::update(Scene *scene)
		{
			if (bobbing)
			{
				z -= sin(bob_pos) * bob_mod;
				bob_pos += bob_speed * tdiff;
				z += sin(bob_pos) * bob_mod;
			}
		}
		
		//----------------------------------------------------------------------
		ThrobbingObject::ThrobbingObject(const char *mesh, float x, float y, 
									   float z)
		{
			BobbingObject::BobbingObject(mesh, x, y, z);
			this->mesh = mesh;
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
			throb_speed = 4.0;
			throb_mod = 0.05;
			throbbing = true;
			throb_pos = 0;
		}
		
		//----------------------------------------------------------------------
		void ThrobbingObject::update(Scene *scene)
		{
			BobbingObject::update(scene);
			if (throbbing)
			{
				scale.x -= sin(throb_pos) * throb_mod;
				scale.y -= sin(throb_pos) * throb_mod;
				scale.z -= sin(throb_pos) * throb_mod;
				throb_pos += throb_speed * tdiff;
				scale.x += sin(throb_pos) * throb_mod;
				scale.y += sin(throb_pos) * throb_mod;
				scale.z += sin(throb_pos) * throb_mod;
			}
		}
		
		//----------------------------------------------------------------------
		Scene::Scene()
		{
		
		}
		
		//----------------------------------------------------------------------
		Scene::~Scene()
		{
		
		}
		
		//----------------------------------------------------------------------
		void Scene::clear()
		{
			objects.clear();
			objects_flat.clear();
			meshes.clear();
		}
		
		//--------------------------------------------------------------------
		ObjectID Scene::add(ObjectType type, Object *obj)
		{
			MeshList::iterator pos = meshes.find(obj->mesh);
			if (pos == meshes.end())
			{
				Graphics::Mesh m;
				string filename;
				
				filename = "Meshes/" + obj->mesh + ".obj";
				m.load(filename.c_str());
				
				meshes[obj->mesh] = m;
			}
			
			objects[type].push_back(obj);
			objects_flat.push_back(obj);
			
			return obj->id;
		}
		
		//----------------------------------------------------------------------
		void Scene::remove(ObjectID id)
		{
			bool found = false;
			
			for (TypedObjectLists::iterator list = objects.begin();
				 list != objects.end(); list++)
			{
				for (ObjectList::iterator obj = list->second.begin();
					 obj != list->second.end(); obj++)
				{
					if ((*obj)->id == id)
					{
						list->second.remove(*obj);
						found = true;
						break;
					}
				}
			}
			
			for (ObjectList::iterator obj = objects_flat.begin();
				 obj != objects_flat.end(); obj++)
			{
				if ((*obj)->id == id)
				{
					objects_flat.remove(*obj);
					break;
				}
			}
			
			LOG_WARNING << "Object not found for removal from scene!" << endl;
		}
		
		//----------------------------------------------------------------------
		void Scene::update()
		{
			for (ObjectList::iterator obj = objects_flat.begin();
				 obj != objects_flat.end(); obj++)
			{
				(*obj)->update(this);
			}
		}
		
		//----------------------------------------------------------------------
		void Scene::render()
		{	
			for (ObjectList::iterator obj = objects_flat.begin();
				 obj != objects_flat.end(); obj++)
			{
				if ((*obj)->do_render)
				{
					(*obj)->render(this);
				}
			}
		}
	}
}
