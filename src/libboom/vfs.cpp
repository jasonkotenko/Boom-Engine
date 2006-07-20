/*
	Virtual Filesystem Implementation
*/

#include <vector>
#include <unistd.h>
#include <bzlib.h>

#include "log.h"
#include "vfs.h"

using namespace std;

namespace Boom
{
	namespace VirtualFS
	{
		vector <MountPoint> mtab;
		
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
			fstream *file;
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
							file = new fstream();
							fstream::openmode fmode;
							switch(mode)
							{
								case MODE_READ:
									fmode = fstream::in;
									break;
								case MODE_WRITE:
									fmode = fstream::out;
									break;
								case MODE_READ_BINARY:
									fmode = fstream::in | fstream::binary;
									break;
								case MODE_WRITE_BINARY:
									fmode = fstream::out | fstream::binary;
									break;
								default:
									LOG_WARNING << "Unknown file mode, "
												<< "defaulting to MODE_READ..."
												<< endl;
									fmode = fstream::in;
									break;
							}
							file->open(fullname.c_str(), fmode);
							
							if (!file)
							{
								LOG_ERROR << "Unable to open " << filename << endl;
								return NULL;
							}
							return file;
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
