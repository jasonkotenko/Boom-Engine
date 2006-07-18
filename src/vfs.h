/*
	Virtual Filesystem
	
	Copyright 2006 Daniel G. Taylor
*/

#ifndef _VFS_H
#define _VFS_H

#include <string>
#include <vector>

#include <unistd.h>
#include <bzlib.h>

#include "log.h"

using namespace std;

namespace Boom
{
	namespace VirtualFS
	{	
		enum MountPointType
		{
			TYPE_DIRECTORY,
			TYPE_BZIP_TAR
		};
		
		struct MountPoint
		{
			string			path;
			MountPointType	type;
		};
		
		enum FileMode
		{
			MODE_READ,
			MODE_WRITE,
			MODE_READ_BINARY,
			MODE_WRITE_BINARY
		};
		
		typedef fstream File;
		
		void init();
		void cleanup();
		
		void mount(string location);
		void umount(string location);
		
		File *open(string filename, FileMode mode = MODE_READ);
	}
}

#endif