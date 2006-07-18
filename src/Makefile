LIB_CXXFLAGS = `sdl-config --cflags` `pkg-config --cflags libpng sigc++-2.0`
LIB_LDFLAGS = `sdl-config --libs` `pkg-config --libs libpng sigc++-2.0` -lbz2

OSX_CXXFLAGS = -O3 $(LIB_CXXFLAGS) -D_OSX_
OSX_LDFLAGS = -framework OpenGL -framework GLUT $(LIB_LDFLAGS)

LINUX_CXXFLAGS = -O3 $(LIB_CXXFLAGS)
LINUX_LDFLAGS = -lGL -lGLU -lglut $(LIB_LDFLAGS) -L/usr/X11R6/lib -lXmu -lXi

# Uncomment these for OSX
CXXFLAGS = $(OSX_CXXFLAGS)
LDFLAGS = $(OSX_LDFLAGS)

# Uncomment these for Linux
#CXXFLAGS = $(LINUX_CXXFLAGS)
#LDFLAGS = $(LINUX_LDFLAGS)

OBJECTS =	log.o \
			vfs.o \
			event.o \
			stringfuncs.o \
			loadpng.o \
			graphics.o

all::test

test: $(OBJECTS)

clean:
	rm -rf *.o core test
