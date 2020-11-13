#!/bin/bash
xhost +
docker run -it --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix  --user user --device=/dev/video0:/dev/video0 ezsocket:latest