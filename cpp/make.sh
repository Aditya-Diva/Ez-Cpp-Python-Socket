#!/bin/bash
g++ ezcppsocket.cpp main.cpp -o client `pkg-config --cflags --libs opencv4`

g++ ezcppsocket.cpp main_server.cpp -o main_server `pkg-config --cflags --libs opencv4`
g++ ezcppsocket.cpp main_client.cpp -o main_client `pkg-config --cflags --libs opencv4`