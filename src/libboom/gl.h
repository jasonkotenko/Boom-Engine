/*
	Platform specific OpenGL includes
*/

#ifndef _GL_H
#define _GL_H

#ifdef _OSX_
	#include <OpenGL/GL.h>
	#include <OpenGL/GLU.h>
	#include <GLUT/GLUT.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
#endif

#endif
