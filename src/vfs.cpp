/*
	Virtual Filesystem Implementation
*/

#include "vfs.h"

namespace Boom
{
	namespace VirtualFS
	{
		vector <MountPoint> mtab;
		
		/*File::File()
		{
			fd = NULL;
		}
		
		File::~File()
		{
		
		}*/
		
		
		
		void init()
		{
			char cwd[255];
			
			LOG_INFO << "Initializing virtual filesystem..." << endl;
			
			mount(getcwd(cwd, 255));
		}
		
		void cleanup()
		{
		
		}
		
		void mount(string location)
		{
			MountPoint mpoint;
			
			LOG_INFO << "Mounting " << location << endl;
			
			mpoint.path = location;
			mpoint.type = TYPE_DIRECTORY;
			
			mtab.push_back(mpoint);
		}
		
		void umount(string location)
		{
			for (vector <MountPoint>::iterator point = mtab.begin();
												point != mtab.end(); point++)
			{
				if (point->path == location)
				{
					mtab.erase(point);
					break;
				}
			}
		}
		
		File *open(string filename, FileMode mode)
		{
			fstream *infile;
			string fullname;
			
			for(vector <MountPoint>::iterator point = mtab.begin();
												point != mtab.end(); point++)
			{
				switch(point->type)
				{
					case TYPE_DIRECTORY:
						fullname = point->path + "/" + filename;
						if (!access(fullname.c_str(), F_OK))
						{
							infile = new fstream();
							switch(mode)
							{
								case MODE_READ:
									infile->open(fullname.c_str());
									break;
								case MODE_READ_BINARY:
									infile->open(fullname.c_str(), fstream::in | fstream::binary);
									break;
							}
							
							if (!infile)
							{
								LOG_ERROR << "Unable to open " << filename << endl;
								return NULL;
							}
							return infile;
						}
						break;
					case TYPE_BZIP_TAR:
						break;
				}
			}
			
			return NULL;
		}
	}
}