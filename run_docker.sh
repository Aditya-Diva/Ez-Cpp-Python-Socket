#!/bin/bash
xhost +
docker run -it --rm -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY  ezsocket:latest