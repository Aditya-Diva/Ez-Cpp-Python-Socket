#include <iostream>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>

// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <sstream>
#include <vector>

#ifndef __EZCPPSOCKET__
#define __EZCPPSOCKET__
/**
 * @brief Python - Cpp Communication Server Object
 * 
 */
class EzCppClient
{
private:
	int sock;					  // Socket point
	std::string server_address;	  // Server address
	int server_port;			  // Port number
	int socket_family;			  // IPV4/IPV6
	int socket_type;			  // TCP/UDP
	bool debug;					  // Debug flag
	struct sockaddr_in serv_addr; // Address struct

public:
	EzCppClient(std::string server_address = "127.0.0.1",
			  int server_port = 10000,
			  int socket_family = AF_INET, // AF_INET6
			  int socket_type = SOCK_STREAM,
			  bool debug = true,
			  bool auto_connect = true);
	~EzCppClient();
	void establishConnect();
	void Disconnect();

	// Incoming

	std::pair<bool, bool> readBool();
	std::string readString();
	int readInt(const int buffer_size = 16);
	float readFloat(const int buffer_size = 16);
	std::vector<int> readIntList();
	std::vector<float> readFloatList();
	cv::Mat readImage();

	// Outgoing

	void sendBool(bool data);
	void sendString(std::string msg);
	void sendInt(int data);
	void sendFloat(float data);
	void sendIntList(std::vector<int> data);
	void sendFloatList(std::vector<float> data);
	void sendImage(cv::Mat img, bool send_size_first = true);
};

#endif