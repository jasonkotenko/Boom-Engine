/*
	PNG Image Loader Implementation
	See loadpng.h for more information!
	
	Copyright 2006 Daniel G. Taylor
*/

#include <iostream>
#include <cstdlib>

#include "loadpng.h"

#include "vfs.h"

using namespace std;
using namespace Boom;

#define ERROR_MEM 4

extern "C" void vfs_png_read(png_structp png_ptr, png_bytep data, png_uint_32 size)
{
	VirtualFS::File *infile = static_cast<VirtualFS::File*>(png_get_io_ptr(png_ptr));

	//LOG_DEBUG << "Reading " << size << " bytes at " << infile->tellg() << endl;

	infile->read((char *)data, size);
	
	if (!infile)
	{
		png_error(png_ptr, "Read Error!");
	}
}

int load_png(const char *filename, PNGData &data)
{
	VirtualFS::File *infile;
	png_structp		png_ptr;
	png_infop 		info_ptr;
	unsigned char	*image_data;
	char			sig[8];
	int				color_type;
	unsigned int	row_bytes;
	png_bytepp		row_pointers;
	
	image_data = NULL;
	
	// Open the file
	infile = VirtualFS::open(filename, VirtualFS::MODE_READ_BINARY);
	if (!infile)
	{
		return 0;
	}
	
	// Read the magic bytes and make sure we are a PNG file!
	infile->read(sig, 8);
	if (!png_check_sig((unsigned char *) sig, 8))
	{
		infile->close();
		delete infile;
		return 0;
	}
	
	// Create the read and info structures
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		infile->close();
		delete infile;
		return ERROR_MEM;
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		infile->close();
		delete infile;
		return ERROR_MEM;
	}
	
	// Setup an error handler
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		infile->close();
		delete infile;
		return 0;
	}
	
	// Tell libpng what file to read and that we have already read 8 bytes
	png_set_read_fn(png_ptr, infile, vfs_png_read);
	png_set_sig_bytes(png_ptr, 8);
	
	// Read the image info and header
	png_read_info(png_ptr, info_ptr);
	
	png_get_IHDR(png_ptr, info_ptr, &(data.width), &(data.height),
				&(data.bit_depth), &color_type, NULL, NULL, NULL);
	
	// Check some options of the image
	if (color_type & PNG_COLOR_MASK_ALPHA)
	{
		data.alpha = true;
	}
	else
	{
		data.alpha = false;
	}
	
	if (data.bit_depth > 8)
	{
		png_set_strip_16(png_ptr);
		data.bit_depth = 8;
	}

	if (color_type == PNG_COLOR_TYPE_GRAY || 
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(png_ptr);
	}
	
	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(png_ptr);
	}
	
	// Update the png info structure if we changed anything above
	png_read_update_info(png_ptr, info_ptr);
	
	// Get the rowsize in bytes
	row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	
	// Allocate memory for the image data
	if ((image_data = (unsigned char *) malloc(row_bytes * data.height))==NULL)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return ERROR_MEM;
	}

	if ((row_pointers = (png_bytepp) malloc(data.height * sizeof(png_bytep))) == NULL)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		free(image_data);
		image_data = NULL;
		return ERROR_MEM;
	}
	
	// Flip the image row pointers so the image starts in the bottom left
	for (int i = 0;  i < data.height;  i++)
	{
		row_pointers[data.height - 1 - i] = image_data + (i * row_bytes);
	}
	
	// Read the actual image data
	png_read_image(png_ptr, row_pointers);
	
	// Cleanup
	free(row_pointers);
	
	infile->close();
	delete infile;
	
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	data.data = image_data;
	
	return 0;
}
