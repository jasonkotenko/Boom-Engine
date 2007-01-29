/*
	Virtual Filesystem
	
	Copyright 2006 Daniel G. Taylor
*/

#ifndef _VFS_H
#define _VFS_H

#include <string>
#include <fstream>
#include <streambuf>

#include "tarfile.h"

using namespace std;

namespace Boom
{
	/// The virtual file system.
	/*!
		An abstraction layer to file system access used within the engine.
		Directories and compressed files can be mounted 
	*/
	namespace VirtualFS
	{
		/// A mount point type
		/*!
			Mount point type, either a directory or compressed archive.
		*/
		enum MountPointType
		{
			TYPE_DIRECTORY,
			TYPE_BZIP_TAR
		};
		
		/// A mount point
		/*!
			Mount point in the virtual file system
		*/
		struct MountPoint
		{
			string			path;
			MountPointType	type;
		};
		
		/// A file mode (e.g. read/write)
		/*!
			The mode a file is opened with (e.g. read, write, etc).
		*/
		enum FileMode
		{
			MODE_READ,
			MODE_WRITE,
			MODE_READ_BINARY,
			MODE_WRITE_BINARY
		};
		
		typedef fstream File;
		
		//! Initialize the virtual file system and mount the current directory
		void init();
		
		//! Unmount everything and cleanup
		void cleanup();
		
		//! Mount a location in the virtual file system
		void mount(string location);
		//! Unmount a location in the virtual file system
		void umount(string location);
		
		//! Open a file through the virtual file system
		File *open(string filename, FileMode mode = MODE_READ);
	}
}

#endif
