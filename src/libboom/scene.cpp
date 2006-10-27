/*
	Scene implementation
*/

#include "log.h"
#include "scene.h"

namespace Boom
{
	namespace Scene
	{
		static ObjectID id = 0;
		
		//----------------------------------------------------------------------
		Object::Object()
		{
			x = y = z = 0;
			do_render = true;
			collision_type = COLLISION_CONVEX_HULL;
		}
		
		//----------------------------------------------------------------------
		Object::Object(const char *mesh, float x, float y, float z)
		{
			this->mesh = mesh;
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
			collision_type = COLLISION_CONVEX_HULL;
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
			
			obj->id = ++id;
			
			objects[type].push_back(obj);
			objects_flat.push_back(obj);
			
			return id;
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
					glPushMatrix();
					glTranslatef((*obj)->x, (*obj)->y, (*obj)->z);
					meshes[(*obj)->mesh].render();
					glPopMatrix();
				}
			}
		}
	}
}