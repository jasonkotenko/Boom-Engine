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
		Object::Object(const char *mesh, float x, float y, float z)
		{
			id = gid++;
			this->mesh = mesh;
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = (mesh != NULL);
			collision_type = COLLISION_CONVEX_HULL;
			rotation.x = rotation.y = rotation.z = 0;
			scale.x = scale.y = scale.z = 1.0;
			life = -1;
		}
		
		//----------------------------------------------------------------------
		Object::~Object()
		{
		
		}
		
		//----------------------------------------------------------------------
		bool Object::update(Scene *scene)
		{
			if (life > 0)
			{
				life -= tdiff;
				if (life <= 0)
					return timeout();
			}
			
			return true;
		}
		
		//----------------------------------------------------------------------
		bool Object::timeout()
		{
			return false;
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
			this->mesh = mesh;
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
			speed = 0;
			moving = false;
		}
		
		//----------------------------------------------------------------------
		bool MovableObject::update(Scene *scene)
		{
			if (moving)
			{
				x += cos((rotation.z - 90) * M_PI / 180.0) * speed * tdiff;
				y += sin((rotation.z - 90) * M_PI / 180.0) * speed * tdiff;
			}
			
			return Object::update(scene);
		}
		
		//----------------------------------------------------------------------
		SimpleAnimatedObject::SimpleAnimatedObject(const char *mesh, float x,
												   float y, float z)
		{
			this->mesh = mesh;
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
			
			rot_speed = 1.0;
			rotating = false;
			
			bob_speed = 3.0;
			bob_mod = 0.1;
			bobbing = false;
			
			throb_speed = 4.0;
			throb_mod = 0.05;
			throbbing = false;
			
			sin_pos = 0;
		}
		
		//----------------------------------------------------------------------
		bool SimpleAnimatedObject::update(Scene *scene)
		{
			if (rotating)
			{
				rotation.z += rot_speed * 30 * tdiff;
				if (rotation.z > 360)
					rotation.z -= 360;
			}
			if (bobbing)
			{
				z -= sin(sin_pos) * bob_mod;
				sin_pos += bob_speed * tdiff;
				z += sin(sin_pos) * bob_mod;
			}
			if (throbbing)
			{
				scale.x -= sin(sin_pos) * throb_mod;
				scale.y -= sin(sin_pos) * throb_mod;
				scale.z -= sin(sin_pos) * throb_mod;
				sin_pos += throb_speed * tdiff;
				scale.x += sin(sin_pos) * throb_mod;
				scale.y += sin(sin_pos) * throb_mod;
				scale.z += sin(sin_pos) * throb_mod;
			}
			
			return Object::update(scene);
		}
		
		//----------------------------------------------------------------------
		BombObject::BombObject(float x, float y, float z)
		{
			this->mesh = "bomb";
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
			bobbing = true;
			throbbing = true;
			life = 5;
			exploding = false;
		}
		
		//----------------------------------------------------------------------
		bool BombObject::update(Scene *scene)
		{
			if (!exploding)
				return SimpleAnimatedObject::update(scene);
			else
			{
				current_radius += tdiff * ((radius - current_radius) + 0.5) * 2;
				if (current_radius >= radius)
					return false;
				return Object::update(scene);
			}
		}
		
		//----------------------------------------------------------------------
		void BombObject::render(Scene *scene)
		{
			if (!exploding)
				SimpleAnimatedObject::render(scene);
			else
			{
				float percent = current_radius / radius;
				
				glPushMatrix();
				glTranslatef(x, y, z + 0.5);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glDisable(GL_DEPTH_TEST);
				
				if (percent <= 0.8)
				{
					float large[] = {0.7, 0.7, 1.0, 0.5 * (0.8 - percent)};
					glMaterialfv(GL_FRONT, GL_DIFFUSE, large);
					glutSolidSphere(current_radius * current_radius * current_radius, 16, 16);
				}
				
				float medium[] = {0.8, 0.8 * percent, 0.8 * percent, 1.0 - percent};
				glMaterialfv(GL_FRONT, GL_DIFFUSE, medium);
				glutSolidSphere(current_radius, 16, 16);
				
				float small[] = {1.0, 1.0, 0.8 * (1.0 - percent), 1.0 - percent};
				glMaterialfv(GL_FRONT, GL_DIFFUSE, small);
				glutSolidSphere(0.5, 12, 12);
				
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);
				glPopMatrix();
			}
		}
		
		//----------------------------------------------------------------------
		bool BombObject::timeout()
		{
			//LOG_INFO << "Bomb (id = " << id << ") timeout called." << endl;
		
			if (!exploding)
			{
				exploding = true;
				radius = 2;
				current_radius = 0.5;
				return true;
			}
			
			return false;
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
			
			if (!found)
				LOG_WARNING << "Object not found for removal from scene!"
							<< endl;
		}
		
		//----------------------------------------------------------------------
		void Scene::update()
		{
			vector <ObjectID> rem_ids;
			
			for (ObjectList::iterator obj = objects_flat.begin();
				 obj != objects_flat.end(); obj++)
			{
				if (!(*obj)->update(this))
					rem_ids.push_back((*obj)->id);
			}
			
			for (vector <ObjectID>::iterator i = rem_ids.begin();
				 i != rem_ids.end(); i++)
			{
				remove(*i);
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
