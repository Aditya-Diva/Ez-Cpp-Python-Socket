#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <sstream>
#include <vector>

#ifndef __PYCCLIENT__
#define __PYCCLIENT__
/**
 * @brief Python - Cpp Communication Server Object
 * 
 */
class PyCClient
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
	/**
		 * @brief Construct a new Py C Client object
		 * 
		 * @param server_address // Server address
		 * @param server_port // Port number
		 * @param socket_family // IPV4/IPV6
		 * @param socket_type // TCP/UDP
		 * @param debug // Debug flag
		 * @param auto_connect // If True, tries to connect directly on object initialization (blocks execution)
		 */
	PyCClient(std::string server_address = "127.0.0.1",
			  int server_port = 10000,
			  int socket_family = AF_INET, // AF_INET6
			  int socket_type = SOCK_STREAM,
			  bool debug = false,
			  bool auto_connect = true);
	/**
		 * @brief Destroy the Py C Client object
		 * 
		 */
	~PyCClient();

	/**
		 * @brief Connect to the socket explicitly
		 * Useful if you want to connect at a point much after
		 * object initialization
		 */
	void establishConnect();

	// Incoming

	/**
		 * @brief Read message received on port
		 * 
		 * @param buffer_size Size of buffer to be read (in bytes)
		 * @return std::string  Received buffer
		 */
	std::string readString();

	/**
		 * @brief Read integer value received on port
		 * 
		 * @return int Received integer
		 */
	int readInt(const int buffer_size = 16);

	/**
		 * @brief Read a list of integer values
		 * 
		 * @return std::vector<int> Integer List received
		 */
	std::vector<int> readIntList();

	/**
		 * @brief Read an OpenCV Image
		 * 
		 * @param receive_size_first Bool flag to get image size from other end
		 * @param buffer_size  If bool flag is not set to true, set a buffer size
		 * as needed
		 * @return cv::Mat Received image 
		 */
	cv::Mat readImage();

	// Outgoing

	void sendString(std::string msg);

	void sendInt(int data);

	/**
		 * @brief Send Image
		 * 
		 * @param img Image to be sent
		 * @param send_size_first Whether size message should be sent first
		 */
	void sendImage(cv::Mat img, bool send_size_first = true);
};

#endif