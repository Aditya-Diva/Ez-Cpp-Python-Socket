# Ez-Cpp-Python-Socket
> Creating tools to enable cpp python socket communication. 

> This is source of developed library that attempts to simplify TCP/IP socket communication. Here, the server is setup on Python and C++/Cpp acts as the client.

Types of messages that can be sent to and fro are as shown below:
```
Type       CPP    Python            Extra Info
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
Preferably use virtual environments.

Linux:
Python Dependencies
```
pip3 install -r python/requirements.txt
```
C++ Dependencies : Install [OpenCV](https://github.com/opencv/opencv)


## Demo
Assuming that OpenCV has already been installed and accessible by C++.
Run shell script to download python dependencies and run demo.
To run demo of server and client together,
```
./run.sh
```

## Usage Example
To inspect usage of commands,
Refer to run.py in [python](python) folder & main.py in [cpp](cpp) folder.

## Meta

Aditya Divakaran - [@LinkedIn](https://www.linkedin.com/in/aditya-divakaran/) - [@Github](https://github.com/Aditya-Diva) - [@GMail](adi.develops@gmail.com)

Note:
- This was developed on Ubuntu 20.04 in a virtual environment. 
- This is a fun project that was picked up while understanding socket communication for other projects that I'm tinkering with.