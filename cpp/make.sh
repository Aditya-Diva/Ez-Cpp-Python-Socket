#!/bin/bash
g++ ezcppsocket.cpp main.cpp -o client `pkg-config --cflags --libs opencv4`