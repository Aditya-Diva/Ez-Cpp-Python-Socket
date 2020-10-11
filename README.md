# Cpp-Python-Socket
Creating tools to enable cpp python socket communication.

This is source of developed library that helps simplify TCP/IP socket communication. Here, the server is setup on python and cpp acts as the client.

Types of messages that can be sent to and fro are as shown below:
```
Type       CPP    Python
========================
int         Y       Y
string      Y       Y
image       Y       Y
list<int>   N       Y   (Python sends list -> vector<int> but not vice versa)
```

To run demo of server and client together,
```
./run.sh
```

To inspect usage of commands,
Refer to run.py in [python](python) folder & main.py in [cpp](cpp) folder.

Note:
- This was developed on Ubuntu 20.04 in a virtual environment. 
- This is a fun project that was picked up while understanding socket communication for
  other projects that I'm tinkering with.