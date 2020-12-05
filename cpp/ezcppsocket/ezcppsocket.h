#include <iostream>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>

// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// To add sleep when checking server address available
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <sstream>
#include <vector>
#include <chrono>

#ifndef __EZCPPSOCKET__
#define __EZCPPSOCKET__
/**
 * @brief Python - Cpp Communication Server Object
 * 
 */
class EzCppSocket
{
private:
	int sock;									// Socket point 
	int fd;										// File descriptor (Server)
	std::string server_address;					// Server address
	int server_port;							// Port number
	int socket_family;							// IPV4/IPV6
	int socket_type;							// TCP/UDP
	bool debug;									// Debug flag
	struct sockaddr_in serv_addr;				// Address struct
	int client_connection_count;				// No. of client connections our server should accept
	float reconnect_on_address_busy;			// No. of seconds timeout before polling again in case of errors raised
	std::pair<std::string, std::string> tokens; // Pair of tokens (start_token, end_token)
	unsigned int sleep_between_packets = 50;	// No. of useconds between reading/sending packets of data
	unsigned int packet_size = 59625;			// No. of bytes in a packet read/write (Should not be more than 65535 (64K))

	bool loop_flag = false;
	unsigned int loop_iteration_count = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> loop_start_time = std::chrono::high_resolution_clock::now();

	void insertTokens(std::string &msg);
	void extractTokens(std::string &msg);
	void pollingTimeout();

public:
	EzCppSocket(std::string server_address = "127.0.0.1",
				int server_port = 10000,
				int socket_family = AF_INET, // AF_INET6
				int socket_type = SOCK_STREAM,
				bool debug = false,
				bool auto_connect = true,
				int client_connection_count = 1,
				bool server_mode = true,
				float reconnect_on_address_busy = 0,
				std::pair<std::string, std::string> tokens = std::pair<std::string, std::string>("", ""));
	~EzCppSocket();
	void establishConnect();
	void Disconnect();
	void setSleepBetweenPackets(unsigned int microseconds);
	unsigned int getSleepBetweenPackets();
	void setPacketSize(unsigned int number_of_bytes);

	bool getLoopFlag();
	void loop_func_decorator(void (*func_ptr)(EzCppSocket&), bool show_ips);
	void serverLoop(void (*func_ptr)(EzCppSocket&), int loop_count = 0, bool show_ips = true);
	void clientLoop(void (*func_ptr)(EzCppSocket&), int loop_count = 0, bool show_ips = true);
	void stopLoop();
	void resetLoop();

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
	void sendImage(cv::Mat img);
};

#endif