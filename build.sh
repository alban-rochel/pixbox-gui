#!/bin/bash

g++ -L/media/BBB/lib -I/media/BBB/include/SDL  -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer $CFLAGS *.cpp -o pixbox_gui


