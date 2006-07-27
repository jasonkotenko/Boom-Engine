/*
	Tar File Module
	===============
		Objects and functions for reading from Tar files.

		Parts of this code come from the GNU Tar utility source files and the
		POSIX specification (IEEE 1003.1 POSIX) for Tar files.

		Copyright 2006 Daniel G. Taylor
*/

#ifndef _TAR_FILE_H
#define _TAR_FILE_H

#include <string>
#include <vector>
#include <fstream>
#include <streambuf>

#include "log.h"

using namespace std;

namespace Boom
{
	namespace VirtualFS
	{
		const int TAR_BLOCK_SIZE = 512;
		const int TAR_FILE_ERROR = 1;

		struct PosixHeader
		{					/* byte offset */
			char name[100];	/*   0 */
			char mode[8];		/* 100 */
			char uid[8];		/* 108 */
			char gid[8];		/* 116 */
			char size[12];		/* 124 */
			char mtime[12];	/* 136 */
			char chksum[8];	/* 148 */
			char typeflag;		/* 156 */
			char linkname[100]; /* 157 */
			char magic[6];		/* 257 */
			char version[2];	/* 263 */
			char uname[32];	/* 265 */
			char gname[32];	/* 297 */
			char devmajor[8];	/* 329 */
			char devminor[8];	/* 337 */
			char prefix[155];	/* 345 */
								/* 500 */
		};

		union TarBlock
		{
			char raw[512];
			PosixHeader header;
		};

		struct TarFileInfo
		{
			string name;
			long offset;
			long size;
		};
		
		class TarFile: public streambuf
		{
			
		};
		
		class TarArchive
		{
			public:
				// Constructors / Destructors
				TarArchive();
				TarArchive(const char *filename);
				virtual ~TarArchive();
				
				// Operators
				operator bool () const;
				bool operator! () const;
				
				// Open a tar file
				int open(const char *filename);
				
				// The name of the file that is opened
				string filename;
				// Info about all files in this archive
				vector <TarFileInfo> files;

			protected:
				// Read info about files in this archive
				int read_headers();
				
				// Internal file pointer
				ifstream infile;
		};
	}
}

#endif
