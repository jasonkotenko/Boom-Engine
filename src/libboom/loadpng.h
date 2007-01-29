/*
	PNG Image Loader
	================
		Functions for loading Portable Network Graphics (PNG) files. This module
		consists of a structure containing information about a image and the actual
		image data, and a function to fill that structure from a file.
		
		Usage Example
		-------------
			PNGData data;
			int result;
			
			result = load_png("myimage.png", &data);
			
			cout << "Width: " << data.width << endl;
			cout << "Height: " << data.height << endl;
		
		Copyright 2006 Daniel G. Taylor
*/

#ifndef _LOADPNG_H
#define _LOADPNG_H

#include <png.h>

/// A PNG File
/*!
	Stores metadata and the pixel data about a Portable Network Graphics file.
*/
struct PNGData
{
	unsigned long width, height;
	int bit_depth;
	bool alpha;
	unsigned char *data;
};

//! Load a PNG from a file
int load_png(const char *filename, PNGData &data);

#endif
