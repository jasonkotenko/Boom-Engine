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

class TestState: public State::State
{
	public:
		TestState()
		{
			scene.add("simpleplane", 0, 0, 0);
			scene.add("player", 3, 2, 0);
			scene.add("player", -3, 1, 0);
			scene.add("player", 3, -3, 0);
			scene.add("bomb", -2, -1, 0);
		}
		
		~TestState() {}
		
		void update()
		{
			if (running)
			{
				
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
		}
		
		void key_pressed(int key)
		{
			switch(key)
			{
				case 27:
					Event::post(EVENT_QUIT);
					break;
				default:
					LOG_INFO << "Key pressed: " << key << endl;
			}
		}
		
		void key_released(int key)
		{
		
		}
		
	private:
		Graphics::Scene scene;
};

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

int main(int argc, char *argv[])
{	
	Boom::init();
	
	LOG_DEBUG << "Using BOOM version " << Boom::VERSION << " compiled on " << Boom::COMPILE_DATE << " " << Boom::COMPILE_TIME << endl;
	
	char path[255];
	getcwd(path, 255);
	
	VirtualFS::mount(string(path) + "/../Demo");
	
	start();
			
	return 0;
}
