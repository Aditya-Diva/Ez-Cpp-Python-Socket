#!/bin/bash
xhost +
docker run --device=/dev/video0:/dev/video0 -it --rm -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY  ezsocket:latest