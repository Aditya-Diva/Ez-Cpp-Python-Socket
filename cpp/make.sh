#!/bin/bash
g++ ezcppsocket.cpp run_server.cpp -o run_server `pkg-config --cflags --libs opencv4`
g++ ezcppsocket.cpp run_client.cpp -o run_client `pkg-config --cflags --libs opencv4`