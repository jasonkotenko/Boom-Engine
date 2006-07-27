/*
	Tar Implementation
*/

#include <cmath>

#include "tarfile.h"

using namespace std;

namespace Boom
{
	namespace VirtualFS
	{
		TarArchive::TarArchive()
		{
			filename = "";
		}

		TarArchive::TarArchive(const char *filename)
		{
			open(filename);
		}

		TarArchive::~TarArchive()
		{

		}

		TarArchive::operator bool () const
		{
			return infile;
		}

		bool TarArchive::operator! () const
		{
			return !infile;
		}

		int TarArchive::open(const char *filename)
		{
			infile.open(filename);
			if (!infile)
			{
				return TAR_FILE_ERROR;
			}
			
			this->filename = filename;
			
			return read_headers();
		}

		int TarArchive::read_headers()
		{
			TarBlock block;
			TarFileInfo info;
			
			// Read a header
			infile.read(block.raw, TAR_BLOCK_SIZE);
			
			while (infile)
			{
				// Save the header data we care about
				info.name = block.header.name;
				info.offset = infile.tellg();
				info.size = strtol(block.header.size, NULL, 8);
				
				if (info.name == "")
					break;
				
				// Save the file info
				files.push_back(info);
				
				// Skip actual file data (rounded up to the nearest blocksize
				infile.seekg(TAR_BLOCK_SIZE * 
							 int(ceil(info.size / float(TAR_BLOCK_SIZE))),
							 ios::cur);
				
				// Read the next header
				infile.read(block.raw, TAR_BLOCK_SIZE);
			}
			
			return 0;
		}
	}
}
