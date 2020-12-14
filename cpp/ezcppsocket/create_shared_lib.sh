#!/bin/bash
VERSION=".0.0.3"
gcc -shared -fPIC -o libezcppsocket.so${VERSION} -I/usr/local/include/opencv4 ezcppsocket.cpp
# ln -s libezcppsocket.so${VERSION} libezcppsocket.so
# cp libezcppsocket.so /usr/local/lib/libezcppsocket.so # To give it access across the system