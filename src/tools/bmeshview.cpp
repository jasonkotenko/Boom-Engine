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
			
			Point3d size = scene.meshes[mesh]->get_size();
			
			float biggest_diff = size.x;
			if (size.y > biggest_diff)
				biggest_diff = size.y;
			if (size.z > biggest_diff)
				biggest_diff = size.z;
			
			//LOG_DEBUG << "Biggest diff is " << biggest_diff << endl;
			
			float distance = (biggest_diff / 2.0) / tan(VIEW_ANGLE / 180.0 * M_PI / 2.0);
			
			LOG_DEBUG << "Distance set to " << distance << endl;
			
			scene.camera.pos.y = -distance;
			scene.camera.pos.z = distance;
			scene.camera.lookat.z = (size.z) / 2.0;
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
				case 32:
					mesh->rotating = !mesh->rotating;
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
