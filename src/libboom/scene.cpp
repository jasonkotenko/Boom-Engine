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
			this->animation = "default";
			this->frame = 0;
			this->frame_timer = 0;
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
			Graphics::BMeshAnimation *anim = scene->meshes[mesh]->animations[animation];
			if (life > 0)
			{
				life -= tdiff;
				if (life <= 0)
					return timeout();
			}
			frame_timer += tdiff;
			
			while (frame_timer >= anim->speed * 0.05)
			{
				frame_timer -= anim->speed * 0.05;
				frame = (frame + 1) % anim->frames.size();
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
			scene->meshes[mesh]->render(animation, frame);
			glPopMatrix();
		}
		
		//----------------------------------------------------------------------
		int Object::set_animation(string name)
		{
			animation = name;
			frame = 0;
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
			float newx = x;
			float newy = y;
			ObjectList remove;
			ObjectList::iterator i;
			bool collision = false;
			float distance, radius;
			
			if (moving)
			{
				newx += cos((rotation.z - 90) * M_PI / 180.0) * speed * tdiff;
				newy += sin((rotation.z - 90) * M_PI / 180.0) * speed * tdiff;
				
				using namespace Graphics;
				
				for (i = scene->objects_flat.begin(); i != scene->objects_flat.end();
					 i++)
				{
					// Don't get against ourself or the level
					if ((*i)->id == id || (*i)->type == TYPE_LEVEL)
						continue;
					// See if the objects' radii are within each other
					distance = distance2d((*i)->x, (*i)->y, newx, newy);
					radius = scene->meshes[(*i)->mesh]->radius + 
							 scene->meshes[mesh]->radius;
					if (distance <= radius * 0.6)
					{
						// Are we moving into or out of the object?
						if (distance <= distance2d((*i)->x, (*i)->y, x, y))
						{
							// TODO: if this is an item we don't have a collision
							// Need to pick up the item and modify player stats
							// For now this is just a collision no matter what
							if (type == TYPE_PLAYER && (*i)->type == TYPE_ITEM)
							{
								// Apply to user
								((Item *) (*i))->apply(this);
								remove.push_back(*i);
							}
							else
							{
								collision = true;
								break;
							}
						}
					}
				}
				
				for (i = remove.begin(); i != remove.end(); i++)
					scene->remove((*i)->id);
				
				if (!collision)
				{
					x = newx;
					y = newy;
				}
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
			
			rotate_speed = 90.0;
			rotating = false;
			
			bob_speed = 3.0;
			bob_mod = 0.1;
			bobbing = false;
			
			throb_speed = 4.0;
			throb_mod = 0.05;
			throbbing = false;
			
			move_speed = 1.0;
			scale_speed = 0.5;
			
			move_to_moving = false;
			rotate_to_rotating = false;
			scale_to_scaling = false;
			
			sin_pos = 0;
		}
		
		//----------------------------------------------------------------------
		bool SimpleAnimatedObject::update(Scene *scene)
		{
			float diff;
			
			if (rotating)
			{
				rotation.z += rotate_speed * tdiff;
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
			
			if (move_to_moving)
			{
				bool moved = false;
				
				if (abs(move_to.x - x) > 0.1)
				{
					x += move_speed * tdiff;
					moved = true;
				}
				if (abs(move_to.y - y) > 0.1)
				{
					y += move_speed * tdiff;
					moved = true;
				}
				if (abs(move_to.z - z) > 0.1)
				{
					z += move_speed * tdiff;
					moved = true;
				}
				
				if (!moved)
					move_to_moving = false;
			}
			
			if (rotate_to_rotating)
			{
				bool rot = false;
				
				if (abs(rotate_to.x - rotation.x) > 0.1)
				{
					rotation.x += rotate_speed * tdiff;
					rot = true;
					if (rotation.x >= 360)
					{
						rotation.x -= 360;
						rotate_to.x -= 360;
						if (rotate_to.x <= 0)
							rotation.x = rotate_to.x;
					}
				}
				if (abs(rotate_to.y - rotation.y) > 0.1)
				{
					rotation.y += rotate_speed * tdiff;
					rot = true;
					if (rotation.y >= 360)
					{
						rotation.y -= 360;
						rotate_to.y -= 360;
						if (rotate_to.y <= 0)
							rotation.y = rotate_to.y;
					}
				}
				if (abs(rotate_to.z - rotation.z) > 0.1)
				{
					rotation.z += rotate_speed * tdiff;
					rot = true;
					if (rotation.z >= 360)
					{
						rotation.z -= 360;
						rotate_to.z -= 360;
						if (rotate_to.z <= 0)
							rotation.z = rotate_to.z;
					}
				}
				
				if (!rot)
					rotate_to_rotating = false;
			}
			
			if (scale_to_scaling)
			{
				bool scaled = false;
				
				if (abs(scale_to.x - scale.x) > 0.01)
				{
					scale.x += scale_speed * tdiff;
					scaled = true;
				}
				if (abs(scale_to.y - scale.y) > 0.01)
				{
					scale.y += scale_speed * tdiff;
					scaled = true;
				}
				if (abs(scale_to.z - scale.x) > 0.01)
				{
					scale.z += scale_speed * tdiff;
					scaled = true;
				}
				
				if (!scaled)
					scale_to_scaling = false;
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
			life = DEFAULT_BOMB_LIFE;
			exploding = false;
			radius = DEFAULT_BOMB_SIZE;
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
				for (ObjectList::iterator i = scene->objects[TYPE_BOMB].begin();
					 i != scene->objects[TYPE_BOMB].end(); i++)
				{
					if (Graphics::distance2d((*i)->x, (*i)->y, x, y) <= 
						current_radius * current_radius)
					{
						(*i)->timeout();
					}
				}
				
				ObjectList del;
				for (ObjectList::iterator i = scene->objects[TYPE_PLAYER].begin();
					 i != scene->objects[TYPE_PLAYER].end(); i++)
				{
					if (Graphics::distance2d((*i)->x, (*i)->y, x, y) <=
						current_radius * current_radius)
					{
						LOG_DEBUG << "Killing object " << (*i)->id << endl;
						del.push_back(*i);
					}
				}
				
				for (ObjectList::iterator i = scene->objects[TYPE_ITEM].begin();
					 i != scene->objects[TYPE_ITEM].end(); i++)
				{
					if (Graphics::distance2d((*i)->x, (*i)->y, x, y) <=
						current_radius * current_radius)
					{
						if (((Item *)(*i))->parent == id)
							continue;
						LOG_DEBUG << "Killing object " << (*i)->id << endl;
						del.push_back(*i);
					}
				}
				
				for (ObjectList::iterator i = scene->objects[TYPE_BLOCK].begin();
					 i != scene->objects[TYPE_BLOCK].end(); i++)
				{
					if (Graphics::distance2d((*i)->x, (*i)->y, x, y) <=
						current_radius * current_radius)
					{
						LOG_DEBUG << "Killing object " << (*i)->id << endl;
						((BlockObject *) (*i))->spawn_item(scene, id);
						del.push_back(*i);
					}
				}
				
				for (ObjectList::iterator i = del.begin(); i != del.end(); i++)
				{
					scene->remove((*i)->id);
				}
				
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
				glDisable(GL_TEXTURE_2D);
				
				if (percent <= 0.8)
				{
					float large[] = {0.6, 0.6, 1.0, 0.5 * (0.8 - percent)};
					glMaterialfv(GL_FRONT, GL_DIFFUSE, large);
					glutSolidSphere(current_radius * current_radius * current_radius, 16, 16);
				}
				
				float medium[] = {0.9, 0.7 * percent, 0.7 * percent, 1.0 - percent};
				glMaterialfv(GL_FRONT, GL_DIFFUSE, medium);
				glutSolidSphere(current_radius, 16, 16);
				
				float small[] = {1.0, 1.0, 0.8 * (1.0 - percent), 1.0 - percent};
				glMaterialfv(GL_FRONT, GL_DIFFUSE, small);
				glutSolidSphere(0.5, 12, 12);
				
				glEnable(GL_TEXTURE_2D);
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
				life = 0;
				exploding = true;
				current_radius = 0.5;
				return true;
			}
			
			return false;
		}
		
		//----------------------------------------------------------------------
		Item::Item(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
			bobbing = true;
			rotating = true;
			item_type = static_cast<ItemType> (rand() % ITEM_TYPE_COUNT);
			switch (item_type)
			{
				/*
					TODO: add different models for items
				*/
				case EXTRA_BOMB:
					mesh = "item_extra_bomb";
				default:
					mesh = "item_test";
			}
			parent = 0;
		}
		
		//----------------------------------------------------------------------
		void Item::apply(Object *player)
		{
			Player *p = (Player *) player;
			
			switch(item_type)
			{
				case FASTER:
					p->speed += 1.0;
					LOG_DEBUG << "Picked up speed increase" << endl;
					break;
				case SLOWER:
					if (p->speed > 1.0)
						p->speed -= 1.0;
					LOG_DEBUG << "Picked up speed decrease" << endl;
					break;
				case EXTRA_BOMB:
					p->bomb_bag++;
					LOG_DEBUG << "Picked up extra bomb" << endl;
					break;
				case BIGGER_BOMBS:
					p->bomb_size += 0.5;
					LOG_DEBUG << "Picked up bigger bombs" << endl;
					break;
				case SMALLER_BOMBS:
					p->bomb_size = 1.0;
					LOG_DEBUG << "Picked up smaller bombs" << endl;
					break;
				case BOMB_LONG_LIFE:
					p->bomb_life += 2.0;
					LOG_DEBUG << "Picked up longer bomb life" << endl;
					break;
				case BOMB_SHORT_LIFE:
					if (p->bomb_life > 2.0)
						p->bomb_life -= 2.0;
					LOG_DEBUG << "Picked up shorter bomb life" << endl;
					break;
			}
		}
		
		//----------------------------------------------------------------------
		BlockObject::BlockObject(float x, float y, float z)
		{
			mesh = "block";
			this->x = x;
			this->y = y;
			this->z = z;
			do_render = true;
		}
		
		//----------------------------------------------------------------------
		void BlockObject::spawn_item(Scene *scene, ObjectID parent)
		{
			Item *i = new Item(x, y, z);
			
			i->parent = parent;			
			scene->add(TYPE_ITEM, (Object *) i);
		}
		
		//----------------------------------------------------------------------
		Player::Player(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			mesh = "player";
			speed = DEFAULT_PLAYER_SPEED;
			bomb_bag = 2;
			bomb_size = DEFAULT_BOMB_SIZE;
			bomb_life = DEFAULT_BOMB_LIFE;
			bombs_laid = 0;
		}
		
		//----------------------------------------------------------------------
		bool Player::update(Scene *scene)
		{
			if (moving && animation != "walking")
				set_animation("walking");
			else if (!moving && animation != "default")
				set_animation("default");
			return MovableObject::update(scene);
		}
		
		//----------------------------------------------------------------------
		void Player::lay_bomb(Scene *scene)
		{
			BombObject *obj = new BombObject(x, y, 0);
			
			obj->radius = bomb_size;
			obj->life = bomb_life;
			obj->bob_speed *= 2.0 - (bomb_life / DEFAULT_BOMB_LIFE);
			obj->throb_speed *= 2.0 - (bomb_life / DEFAULT_BOMB_LIFE);
			
			scene->add(TYPE_BOMB, (Object *) obj);
		}
		
		//----------------------------------------------------------------------
		AIPlayer::AIPlayer(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			mesh = "player";
			speed = DEFAULT_PLAYER_SPEED;
			bomb_bag = 2;
			bomb_size = DEFAULT_BOMB_SIZE;
			bomb_life = DEFAULT_BOMB_LIFE;
			bombs_laid = 0;
			current_action = NONE;
			target = NULL;
			decision_timer = 0;
		}
		
		//----------------------------------------------------------------------
		bool AIPlayer::update(Scene *scene)
		{
			decision_timer += tdiff;
			if (decision_timer > 0.1)
			{
				decision_timer = 0;
				select_action(scene);
			}
			
			switch(current_action)
			{
				case ESCAPE:
					// Update the angle we are running to catch something
					rotation.z = polar_angle2d(x, y, target->x, target->y)
								 / M_PI * 180 + 90.0;
					break;
				case CHASE:
					// Update the angle we are running to catch something
					rotation.z = polar_angle2d(x, y, target->x, target->y)
								 / M_PI * 180 - 90.0;
					break;
			}
			
			return Player::update(scene);
		}
		
		//---------------------------------------------------------------------
		void AIPlayer::select_action(Scene *scene)
		{
			ObjectList::iterator obj;
			float closest_distance = 99999;
			
			target = NULL;
			current_action = NONE;
			
			// Run away from a bomb?
			for (obj = scene->objects[TYPE_BOMB].begin();
				 obj != scene->objects[TYPE_BOMB].end(); obj++)
			{
				if (distance2d((*obj)->x, (*obj)->y, x, y) < ((BombObject *)(*obj))->radius)
				{
					target = (*obj);
					current_action = ESCAPE;
					return;
				}
			}
			
			// Find and pick up the closest item
			for (obj = scene->objects[TYPE_ITEM].begin();
				 obj != scene->objects[TYPE_ITEM].end(); obj++)
			{
				if (distance2d((*obj)->x, (*obj)->y, x, y) < closest_distance)
				{
					target = (*obj);
					closest_distance = distance2d((*obj)->x, (*obj)->y, x, y);
				}
			}
			if (target != NULL)
			{
				current_action = CHASE;
				return;
			}
			
			// Find closest player and bomb or chase him
			for (obj = scene->objects[TYPE_PLAYER].begin();
				 obj != scene->objects[TYPE_PLAYER].end(); obj++)
			{
				if ((*obj)->id == id)
					continue;
				
				if (distance2d((*obj)->x, (*obj)->y, x, y) < closest_distance)
				{
					target = (*obj);
					closest_distance = distance2d((*obj)->x, (*obj)->y, x, y);
				}
			}
			if (closest_distance >= bomb_size)
			{
				current_action = CHASE;
			}
			else
			{
				lay_bomb(scene);
				current_action = ESCAPE;
			}
			
			moving = true;
			
			//LOG_DEBUG << "Target is " << target->id << endl;
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
		
		//----------------------------------------------------------------------
		void Scene::preload(string mesh)
		{
			Graphics::BMesh *m = new Graphics::BMesh();
			string filename;
			
			filename = "Meshes/" + mesh + ".bmesh";
			m->load(filename.c_str());
			
			meshes[mesh] = m;
		}
		
		//----------------------------------------------------------------------
		ObjectID Scene::add(ObjectType type, Object *obj)
		{
			MeshList::iterator pos = meshes.find(obj->mesh);
			if (pos == meshes.end())
			{
				Graphics::BMesh *m = new Graphics::BMesh();
				string filename;
				
				filename = "Meshes/" + obj->mesh + ".bmesh";
				m->load(filename.c_str());
				
				meshes[obj->mesh] = m;
			}
			
			obj->type = type;
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
			camera.render();
			
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
