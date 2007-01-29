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
			
			player = new Scene::MovableObject("player", 3, 2, 0);
			player->speed = 3.0;
			obj = (Scene::Object *) player;
			scene.add(Scene::TYPE_PLAYER, obj);
			
			obj = new Scene::Object("player", -3, 1, 0);
			scene.add(Scene::TYPE_PLAYER, obj);
			
			obj = new Scene::Object("player", 3, -3, 0);
			scene.add(Scene::TYPE_PLAYER, obj);
			
			obj = new Scene::Object("bomb", -2, -1, 0);
			scene.add(Scene::TYPE_BOMB, obj);
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
			
			gluLookAt(0, -25, 25, 0, 0, 0, 0, 0, 1);
			
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
		
		void lay_bomb()
		{
			Scene::Object *obj;
			
			obj = new Scene::Object("bomb", player->x, player->y, 0);
			scene.add(Scene::TYPE_BOMB, obj);
		}
		
		void key_pressed(int key)
		{
			switch(key)
			{
				case 27:
					Event::post(EVENT_QUIT);
					break;
				case 276: // left
					movement.left();
					player->update_angle(movement.angle);
					player->moving = movement.moving;
					break;
				case 273: // up
					movement.up();
					player->update_angle(movement.angle);
					player->moving = movement.moving;
					break;
				case 275: // right
					movement.right();
					player->update_angle(movement.angle);
					player->moving = movement.moving;
					break;
				case 274: // down
					movement.down();
					player->update_angle(movement.angle);
					player->moving = movement.moving;
					break;
				case 32: // space
					lay_bomb();
					break;
				default:
					LOG_INFO << "Key pressed: " << key << endl;
			}
		}
		
		void key_released(int key)
		{
			switch (key)
			{
				case 276: // left
					movement.left();
					player->update_angle(movement.angle);
					player->moving = movement.moving;
					break;
				case 273: // up
					movement.up();
					player->update_angle(movement.angle);
					player->moving = movement.moving;
					break;
				case 275: // right
					movement.right();
					player->update_angle(movement.angle);
					player->moving = movement.moving;
					break;
				case 274: // down
					movement.down();
					player->update_angle(movement.angle);
					player->moving = movement.moving;
					break;
			}
		}
		
	private:
		Interface::KeyboardMovement movement;
		Scene::Scene scene;
		Scene::MovableObject *player;
};

//------------------------------------------------------------------------------
void start()
{
	Interface::SDLInterface interface;
	TestState *state = new TestState();
	
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
	
	Boom::init();
	
	Log::set_level(Log::LEVEL_DEBUG);
	
	char path[255];
	getcwd(path, 255);
	
	VirtualFS::mount(string(path) + "/../Demo");
	
	start();
			
	return 0;
}
