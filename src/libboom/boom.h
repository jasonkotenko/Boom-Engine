/*
	Boom Engine
	===========
		A game engine for easily creating bomberman-like games.
		
		Modules
		-------
		Event		 - Event handling and dispatching
		Graphics	 - Classes and functions dealing with graphics objects
					   such as 2d/3d points, materials, meshes, etc.
		Log			 - Generic logging facility
		State		 - Game state management
		VirtualFS	 - Virtual filesystem

		License
		-------
		Copyright (C) 2006 Daniel G. Taylor, Jens Taylor, Jason Kotenko

		This program is free software; you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation; either version 2 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with this program; if not, write to the Free Software
		Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

/*! \mainpage

	\section about About
	Boom is an engine for creating bomberman-like games.
	
	\section simple_usage Simple Usage
	Here is a quick example:
	
	\code
	#include "boom.h"
	
	using namespace Boom;
	
	int main()
	{
		// Initialize Boom
		init();
	}
	\endcode
*/

#ifndef _BOOM_H
#define _BOOM_H

#include "log.h"
#include "vfs.h"
#include "event.h"
#include "audio.h"
#include "graphics.h"
#include "scene.h"
#include "state.h"
#include "interface.h"
#include "sdl_interface.h"

/// The main Boom Engine namespace.
/*!
	This contains all of the engine functions and objects that can be used when
	creating a game.
*/
namespace Boom
{
	extern const float VERSION;

	extern const char *COMPILE_DATE;
	extern const char *COMPILE_TIME;
	
	//! Initialize all modules
	void init(int &argc, char *argv[]);
	
	//! Cleanup all modules
	void cleanup();
}

#endif
