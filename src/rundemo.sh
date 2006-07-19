#!/bin/bash

make
LD_LIBRARY_PATH=. ./test
