/*
	Program Description Here
*/

#include <iostream>
#include <cmath>

#include "boom.h"

using namespace std;
using namespace Boom;

//------------------------------------------------------------------------------
class ModelViewState: public State::State
{
	public:
		ModelViewState(string mesh = "player", string animation = "default")
		{
			this->mesh = new Scene::SimpleAnimatedObject(mesh.c_str(), 0, 0, 0);
			this->mesh->rotate_speed = 45;
			this->mesh->rotating = true;
			this->mesh->animation = animation;
			scene.add(Scene::TYPE_CUSTOM, (Scene::Object *) this->mesh);
			
			Graphics::Point3d low, high;
			Graphics::BMeshFrame *frame = scene.meshes[mesh]->animations["default"]->frames[0];
			
			vector <Graphics::Point3d>::iterator i;
			
			for (i = scene.meshes[mesh]->vertices.begin(); i != scene.meshes[mesh]->vertices.end(); i++)
			{
				if (i->x < low.x)  low.x = i->x;
				if (i->y < low.y)  low.y = i->y;
				if (i->z < low.z)  low.z = i->z;
				if (i->x > high.x) high.x = i->x;
				if (i->y > high.y) high.y = i->y;
				if (i->z > high.z) high.z = i->z;
			}
			
			float biggest_diff = high.x - low.x;
			if (high.y - low.y > biggest_diff)
				biggest_diff = high.y - low.y;
			if (high.z - low.z > biggest_diff)
				biggest_diff = high.z - low.z;
			
			//LOG_DEBUG << "Low: " << low.x << ", " << low.y << ", " << low.z << endl;
			//LOG_DEBUG << "High: " << high.x << ", " << high.y << ", " << high.z << endl;
			
			//LOG_DEBUG << "Biggest diff is " << biggest_diff << endl;
			
			float distance = (biggest_diff / 2.0) / tan(VIEW_ANGLE / 180.0 * M_PI / 2.0);
			
			LOG_DEBUG << "Distance set to " << distance << endl;
			
			scene.camera.pos.y = -distance;
			scene.camera.pos.z = distance;
			scene.camera.lookat.z = (high.z - low.z) / 2.0;
		}
		
		~ModelViewState()
		{
		
		}
		
		void update()
		{
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
					Event::post(EVENT_QUIT);
					break;
			}
		}
		
		void key_released(int key)
		{
		
		}
		
	private:
		Scene::Scene scene;
		Scene::SimpleAnimatedObject *mesh;
};

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	cout << "Using BOOM version " << Boom::VERSION << " compiled on " << Boom::COMPILE_DATE << " " << Boom::COMPILE_TIME << endl;
	
	Boom::init(argc, argv);
	
	Log::set_level(Log::LEVEL_DEBUG);
	
	char path[255];
	getcwd(path, 255);
	
	VirtualFS::mount(string(path) + "/../Demo");
	
	LOG_INFO << "Viewing " << argv[1] << endl;
	
	Interface::SDLInterface interface(800, 600);
	ModelViewState *state = new ModelViewState(argv[1], argv[2]);
	
	State::push(state);
	
	interface.start();
	
	Boom::cleanup();
	
	return 0;
}
