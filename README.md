# Ez-Cpp-Python-Socket
> Creating tools to enable cpp python socket communication. 

> This is source of developed library that attempts to simplify TCP/IP socket communication.

Types of messages that can be sent to and fro are as shown below:
```
Type       Cpp    Python            Extra Info
==========================================================
bool        Y       Y
string      Y       Y
int         Y       Y
float       Y       Y
list<int>   Y       Y   vector<int>     <->     list
list<float> Y       Y   vector<float>   <->     list
image       Y       Y
```

## Installation
Preferably use dockers or virtual environments.

### Linux:
Python Dependencies
```
./install_dep.sh
```
C++ Dependencies : Install [OpenCV](https://github.com/opencv/opencv)


## Demo

### Docker
```
./build_docker.sh # builds docker image as required.
./run_docker.sh # takes you directly to demo
```

### Local Installation / VirtualEnv
Assuming that OpenCV has already been installed and accessible by C++.
Run shell script to download python dependencies and run demo.
To run demo of server and client together,
```
./run.sh
```

## Usage Example
To inspect usage of commands,
Refer to run_server and run_client scripts in [python](python) & [cpp](cpp) folders.

## Meta

Aditya Divakaran - [@LinkedIn](https://www.linkedin.com/in/aditya-divakaran/) - [@Github](https://github.com/Aditya-Diva) - [@GMail](adi.develops@gmail.com)

Note:
- This was tested on Ubuntu 20.04 in a virtual environment and on docker. 
- This is a fun project that was picked up while understanding socket communication for other projects that I'm tinkering with.