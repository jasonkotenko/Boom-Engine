#!/usr/bin/env python

# Setup python's search path so we can run from svn checkout
import os, sys
sys.path.append(os.getcwd() + "/../")

import Boom

# Create an interface
interface = Boom.Interface.SDLInterface()

# Load a simple level and add a few players
level = Boom.load_level("simple2")
level.add_player("Daniel", 0, 0, True)
level.add_player("CPU 1", 4, 1)
level.add_player("CPU 2", -2, -4)

# Start the engine
interface.start()
