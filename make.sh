#!/bin/bash
g++ client.cpp -o client `pkg-config --cflags --libs opencv4`