#!/bin/bash
g++ pycclient.cpp main.cpp -o client `pkg-config --cflags --libs opencv4`