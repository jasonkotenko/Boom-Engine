#ifndef _MENU_H_
#define _MENU_H_

#include <string>
#include <list>
#include "graphics.h"

using namespace std;

namespace Boom
{
	using namespace Graphics;
	
	namespace Interface
	{	
		enum MenuKeyAction { MENU_KEY_NOT_HANDLED,
							 MENU_KEY_SELECTED,
							 MENU_KEY_MOVEMENT };
	
		class Menu
		{	
			public:
				string	title;
				Color	title_color;
				Color	selected_color;
				Color	color;
			
				Menu();
				void add_item(string name);
				
				MenuKeyAction key_pressed(int key);
				void up();
				void down();
				int get_selected();
				
				void draw();
			
			private:
				list <string> items;
				int selected;
				
				void set_pos(float x, float y);
		};
	}
}

#endif
