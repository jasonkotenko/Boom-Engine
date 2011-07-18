/*
	Boom functionality test application.
	
	Copyright 2006 Daniel G. Taylor
*/

#include <iostream>
#include <string>

#include "SDL.h"

#include "boom.h"

using namespace std;
using namespace Boom;

const double FPS_PRINT_DELAY = 5.0;

double fps_timer;
int fps;

class PausedMenuState: public State::State
{
	public:
		PausedMenuState()
		{
			this->game = Boom::State::current;
			menu.title = "Game Paused";
			menu.add_item("Resume");
			menu.add_item("Exit to menu");
		}
		
		void update()
		{
			
		}
		
		void draw()
		{
			game->draw();
			menu.draw();
		}
		
		void key_pressed(int key)
		{
			Interface::MenuKeyAction action;
			
			action = menu.key_pressed(key);
			
			if (action == Interface::MENU_KEY_SELECTED)
			{
				int item = menu.get_selected();
				switch(item)
				{
					case 0:
						Boom::State::pop();
						break;
					case 1:
						Boom::State::pop(2);
						break;
				}
			}
			else
			{
				switch(key)
				{
					case 27:
						Boom::State::pop();
						break;
				}
			}
		}
		
		void key_released(int key)
		{
			
		}
	
	private:
		Boom::State::State *game;
		Interface::Menu menu;
};

//------------------------------------------------------------------------------
class TestState: public State::State
{
	public:
		TestState()
		{
			// Add some objects into the scene...
			Scene::Object *obj;
			
			obj = new Scene::Object("simpleplane", 0, 0, 0);
			scene.add(Scene::TYPE_LEVEL, obj);
			
			player = new Scene::Player(3, 2, 0);
			obj = (Scene::Object *) player;
			scene.add(Scene::TYPE_PLAYER, obj);
			
			//obj = new Scene::Object("player", -3, 1, 0);
			obj = (Scene::Object *) new Scene::AIPlayer(-3, 1, 0);
			scene.add(Scene::TYPE_PLAYER, obj);
			
			obj = (Scene::Object *) new Scene::SimpleAnimatedObject("bign", 3, -3, 0);
			((Scene::SimpleAnimatedObject *) obj)->rotating = true;
			obj->scale.x = obj->scale.y = obj->scale.z = 0.25;
			scene.add(Scene::TYPE_PLAYER, obj);
			
			obj = (Scene::Object *) new Scene::BombObject(-2, -1, 0);
			scene.add(Scene::TYPE_BOMB, obj);
			
			obj = (Scene::Object *) new Scene::BlockObject(-1, 4, 0);
			scene.add(Scene::TYPE_BLOCK, obj);
			
			obj = (Scene::Object *) new Scene::BlockObject(-1, -4, 0);
			scene.add(Scene::TYPE_BLOCK, obj);
			
			obj = (Scene::Object *) new Scene::BlockObject(3, -1, 0);
			scene.add(Scene::TYPE_BLOCK, obj);
			
			obj = (Scene::Object *) new Scene::BlockObject(-3, -2, 0);
			scene.add(Scene::TYPE_BLOCK, obj);
			
			Audio::play_music("../Demo/Sounds/Menu.ogg");
		}
		
		~TestState() {}
		
		void update()
		{
			if (running)
			{
				scene.update();
			}
		}
		
		void draw()
		{
			double current;
			
			scene.render();
			
			fps++;
			
			current = Interface::seconds();
			if (current >= fps_timer)
			{
				LOG_INFO << "Frames per second: " << fps / FPS_PRINT_DELAY << endl;
				fps = 0;
				fps_timer = current + FPS_PRINT_DELAY;
			}
			
			usleep(1);
		}
		
		void key_pressed(int key)
		{
			PausedMenuState *state;
			
			switch(key)
			{
				case 27:
					//Event::post(EVENT_QUIT);
					state = new PausedMenuState();
					Boom::State::push(state);
					break;
				case 276: // left
					movement.left();
					player->rotation.z = movement.angle;
					player->moving = movement.moving;
					break;
				case 273: // up
					movement.up();
					player->rotation.z = movement.angle;
					player->moving = movement.moving;
					break;
				case 275: // right
					movement.right();
					player->rotation.z = movement.angle;
					player->moving = movement.moving;
					break;
				case 274: // down
					movement.down();
					player->rotation.z = movement.angle;
					player->moving = movement.moving;
					break;
				case 32: // space
					player->lay_bomb(&scene);
					break;
				default:
					LOG_INFO << "Key pressed: " << key << endl;
			}
			if (player->moving)
				player->set_animation("walking");
			else
				player->set_animation("default");
		}
		
		void key_released(int key)
		{
			switch (key)
			{
				case 276: // left
					movement.left();
					player->rotation.z = movement.angle;
					player->moving = movement.moving;
					break;
				case 273: // up
					movement.up();
					player->rotation.z = movement.angle;
					player->moving = movement.moving;
					break;
				case 275: // right
					movement.right();
					player->rotation.z = movement.angle;
					player->moving = movement.moving;
					break;
				case 274: // down
					movement.down();
					player->rotation.z = movement.angle;
					player->moving = movement.moving;
					break;
			}
			if (player->moving)
				player->set_animation("walking");
			else
				player->set_animation("default");
		}
		
	private:
		Interface::KeyboardMovement movement;
		Scene::Scene scene;
		Scene::Player *player;
};

class MainMenuState: public State::State
{
	public:
		MainMenuState()
		{
			menu.title = "Boom Engine";
			menu.add_item("Begin Demo");
			menu.add_item("Exit");
		}
		
		void update()
		{
			
		}
		
		void draw()
		{
			menu.draw();
		}
		
		void key_pressed(int key)
		{
			Interface::MenuKeyAction action;
			
			action = menu.key_pressed(key);
			
			if (action == Interface::MENU_KEY_SELECTED)
			{
				TestState *state;
				int item = menu.get_selected();
				switch(item)
				{
					case 0:
						state = new TestState();
						Boom::State::push(state);
						break;
					case 1:
						Event::post(EVENT_QUIT);
						break;
				}
			}
			else
			{
				switch(key)
				{
					case 27:
						Event::post(EVENT_QUIT);
						break;
				}
			}
		}
		
		void key_released(int key)
		{
			
		}
	
	private:
		Interface::Menu menu;
};

//------------------------------------------------------------------------------
class IntroState: public State::State
{
	public:
		IntroState()
		{
			scene.preload("bign");
			scene.preload("g");
			scene.preload("u");
			scene.preload("freesoftware");
			
			timer = 0;
			
			bign = new Scene::SimpleAnimatedObject("bign", 0, 0, 0);
			bign->scale.x = bign->scale.y = bign->scale.z = 0.01;
			bign->scale_to.x = bign->scale_to.y = bign->scale_to.z = 1.0;
			bign->scale_to_scaling = true;
			bign->rotate_to.z = 360;
			bign->rotate_speed = 180;
			bign->rotate_to_rotating = true;
			scene.add(Scene::TYPE_CUSTOM, (Scene::Object *) bign);
			
			g = new Scene::SimpleAnimatedObject("g", -45, 0, 0);
			g->move_to.x = -6;
			g->move_speed = 18;
			g->move_to_moving = true;
			scene.add(Scene::TYPE_CUSTOM, (Scene::Object *) g);
			
			u = new Scene::SimpleAnimatedObject("u", 45, 0, 0);
			u->move_to.x = 6;
			u->move_speed = -18;
			u->move_to_moving = true;
			scene.add(Scene::TYPE_CUSTOM, (Scene::Object *) u);
			
			fs = new Scene::SimpleAnimatedObject("freesoftware", 0, 0, -10);
			fs->rotation.x = -45;
		}
		
		~IntroState() {}
		
		void update()
		{
			if (!bign->scale_to_scaling && timer == 0)
			{
				bign->throbbing = true;
				scene.add(Scene::TYPE_CUSTOM, (Scene::Object *) fs);
				timer = 1.5;
			}
			
			if (timer)
			{
				timer -= tdiff;
				if (timer <= 0)
				{
					MainMenuState *state = new MainMenuState();
					Boom::State::replace(state);
				}
			}
			
			scene.update();
		}
		
		void draw()
		{
			scene.render();
		}
		
		void key_pressed(int key)
		{
			switch(key)
			{
				case 27:
					MainMenuState *state = new MainMenuState();
					Boom::State::replace(state);
					break;
			}
		}
		
		void key_released(int key)
		{
			
		}
	
	private:
		Scene::Scene scene;
		Scene::SimpleAnimatedObject *bign;
		Scene::SimpleAnimatedObject *g, *u;
		Scene::SimpleAnimatedObject *fs;
		float timer;
};

//------------------------------------------------------------------------------
void start()
{
	Interface::SDLInterface interface(800, 600);
	//TestState *state = new TestState();
	IntroState *state = new IntroState();
	
	State::push(state);
	
	fps = 0;
	fps_timer = Interface::seconds() + FPS_PRINT_DELAY;
	
	interface.start();
	
	Boom::cleanup();
}

//==============================================================================
int main(int argc, char *argv[])
{
	cout << "Using BOOM version " << Boom::VERSION << " compiled on " << Boom::COMPILE_DATE << " " << Boom::COMPILE_TIME << endl;
	
	Boom::init(argc, argv);
	
	Log::set_level(Log::LEVEL_DEBUG);
	
	char path[255];
	getcwd(path, 255);
	
	VirtualFS::mount(string(path) + "/../Demo");
	
	start();
			
	return 0;
}
