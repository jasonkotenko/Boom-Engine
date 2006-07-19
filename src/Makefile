LIB_CXXFLAGS = `sdl-config --cflags` `pkg-config --cflags libpng sigc++-2.0`
LIB_LDFLAGS = `sdl-config --libs` `pkg-config --libs libpng sigc++-2.0` -lbz2

OPTIMIZATIONS = -O2 -fomit-frame-pointer -pipe

OSX_CXXFLAGS = $(OPTIMIZATIONS) $(LIB_CXXFLAGS) -D_OSX_ -Wall
OSX_LDFLAGS = -framework OpenGL -framework GLUT $(LIB_LDFLAGS)

LINUX_CXXFLAGS = $(OPTIMIZATIONS) $(LIB_CXXFLAGS) -Wall -march=i686
LINUX_LDFLAGS = -lGL -lGLU -lglut $(LIB_LDFLAGS) -L/usr/X11R6/lib -lXi

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
