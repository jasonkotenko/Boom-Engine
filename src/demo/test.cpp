/*
	Boom functionality test application.
	
	Copyright 2006 Daniel G. Taylor
*/

#include <iostream>
#include <string>
#include <sys/time.h>

#include <SDL.h>

#include "boom.h"

using namespace std;
using namespace Boom;

const float FPS_PRINT_DELAY = 5.0;

Graphics::Scene scene;
double fps_timer;
int fps;

double seconds()
{
	timeval t;
	double s;
	
	gettimeofday(&t, NULL);
	s = t.tv_sec + (t.tv_usec / 10000000.0);
	
	return s;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	gluLookAt(0, -25, 25, 0, 0, 0, 0, 0, 1);
	
	scene.render();
	
	fps++;
	if (fps_timer <= seconds())
	{
		LOG_INFO << "Frames per second: " << fps / FPS_PRINT_DELAY << endl;
		fps = 0;
		fps_timer = seconds() + FPS_PRINT_DELAY;
	}
	
	SDL_GL_SwapBuffers();
	
	//usleep(1);
}

void quit()
{
	SDL_Quit();
	Boom::cleanup();
	exit(0);
}

void keydown(int key)
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

int main(int argc, char *argv[])
{	
	SDL_Init(SDL_INIT_VIDEO);
	
	Boom::init();
	
	LOG_DEBUG << "Using BOOM version " << Boom::VERSION << " compiled on " << Boom::COMPILE_DATE << " " << Boom::COMPILE_TIME << endl;
	
	char path[255];
	getcwd(path, 255);
	
	VirtualFS::mount(string(path) + "/../Demo");
    
    SDL_Surface *screen;
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    
    screen = SDL_SetVideoMode(640, 480, 0, SDL_HWSURFACE | SDL_OPENGL);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glEnable(GL_LIGHT0);
	float position[] = {50.0, 50.0, 150.0, 0.0};
	float ambient[] = {0.2, 0.2, 0.2};
	float diffuse[] = {1.0, 1.0, 1.0};
	float specular[] = {1.0, 1.0, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glEnable(GL_LIGHTING);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(25.0, 640.0 / 480.0, 10.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	
	scene.add("simpleplane", 0, 0, 0);
	scene.add("player", 3, 2, 0);
	scene.add("player", -3, 1, 0);
	scene.add("player", 3, -3, 0);
	scene.add("bomb", -2, -1, 0);
	
	SDL_Event event;
	
	Event::connect(EVENT_QUIT, quit);
	Event::connect(EVENT_KEY_DOWN, keydown);
	
	fps = 0;
	fps_timer = seconds() + FPS_PRINT_DELAY;
	
	while (1)
	{
		Event::process();
		
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					Event::post(EVENT_QUIT, PRIORITY_HIGH);
					break;
				case SDL_KEYDOWN:
					Event::post(EVENT_KEY_DOWN, event.key.keysym.sym);
			}
		}
		
		display();
	}
	
	return 0;
}
