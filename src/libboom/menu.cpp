#include "menu.h"

namespace Boom
{
	namespace Interface
	{
		//---------------------------------------------------------------------
		Menu::Menu()
		{
			title = "Menu Title";
			title_color.red = title_color.green = title_color.blue = 1.0;
			selected_color.red = 1.0;
			selected_color.green = selected_color.blue = 0.0;
			color.red = color.green = color.blue = 0.8;
			selected = 0;
		}
		
		//---------------------------------------------------------------------
		void Menu::add_item(string name)
		{
			items.push_back(name);
		}
		
		//---------------------------------------------------------------------
		MenuKeyAction Menu::key_pressed(int key)
		{
			switch(key)
			{
				case 273:
					up();
					return MENU_KEY_MOVEMENT;
				case 274:
					down();
					return MENU_KEY_MOVEMENT;
				case 13:
					return MENU_KEY_SELECTED;
			}
		
			return MENU_KEY_NOT_HANDLED;
		}
		
		//---------------------------------------------------------------------
		void Menu::up()
		{
			selected = (selected - 1) % items.size();
		}
		
		//---------------------------------------------------------------------
		void Menu::down()
		{
			selected = (selected + 1) % items.size();
		}
		
		//---------------------------------------------------------------------
		int Menu::get_selected()
		{
			return selected;
		}
		
		//---------------------------------------------------------------------
		void Menu::draw()
		{
			float y;
			float left;
			int pos;
			string::iterator i;
			list <string>::iterator j;
			
			glPushMatrix();
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			y = 0;
			left = -0.2;
			
			// Draw the menu title
			glColor3fv(&title_color.red);
			set_pos(left, y);
			y -= 0.1;
			for (i = title.begin(); i != title.end(); i++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *i);
			
			pos = 0;
			// Draw each menu item
			for (j = items.begin(); j != items.end(); j++)
			{
				if (pos == selected)
					glColor3fv(&selected_color.red);
				else
					glColor3fv(&color.red);
				set_pos(left, y);
				y -= 0.075;
				for (i = j->begin(); i != j->end(); i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *i);
				pos++;
			}
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			glPopMatrix();
		}
		
		//---------------------------------------------------------------------
		void Menu::set_pos(float x, float y)
		{
			glPushAttrib(GL_TRANSFORM_BIT);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glRasterPos2f(x, y);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glPopAttrib();
		}
	}
}
