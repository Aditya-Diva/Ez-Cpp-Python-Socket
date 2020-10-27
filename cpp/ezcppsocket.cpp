#include "ezcppsocket.h"

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
EzCppClient::EzCppClient(std::string server_address,
					 int server_port,
					 int socket_family, // AF_INET6
					 int socket_type,
					 bool debug,
					 bool auto_connect,
					 bool server_mode)
{
	// Dump values in class
	this->server_address = server_address;
	this->server_port = server_port;
	this->socket_family = socket_family;
	this->socket_type = socket_type;
	this->debug = debug;

	this->serv_addr.sin_family = this->socket_family;
	this->serv_addr.sin_port = htons(this->server_port);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(this->socket_family, this->server_address.c_str(), &this->serv_addr.sin_addr) <= 0)
		printf("\nInvalid address/ Address not supported \n");

	if (auto_connect)
	{
		printf("Starting ");
		printf((server_mode?"Server":"Client"));
		printf("...\n");

		printf("Starting up on %s port %s\n", this->server_address.c_str(), std::to_string(this->server_port).c_str());
		if (server_mode){
			int opt = 1;
			int addrlen = sizeof(this->serv_addr); 
			int server_fd;
			
			// Creating socket file descriptor 
			if ((server_fd = socket(this->socket_family, this->socket_type, 0)) == 0) 
			{ 
				perror("Socket creation failed"); 
				exit(EXIT_FAILURE); 
			} 
			else printf("Socket creation successful\n");

			// Forcefully attaching socket to the port 
			if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
														&opt, sizeof(opt))) 
			{ 
				perror("setsockopt"); 
				exit(EXIT_FAILURE); 
			} 
			this->serv_addr.sin_addr.s_addr = INADDR_ANY; 
			
			// Forcefully attaching socket to the port 8080 
			if (bind(server_fd, (struct sockaddr *)&this->serv_addr,  
										sizeof(this->serv_addr))<0) 
			{ 
				perror("bind failed"); 
				exit(EXIT_FAILURE); 
			} 
			if (listen(server_fd, 1) < 0) // queue of pending connections -> 1
			{ 
				perror("listen"); 
				exit(EXIT_FAILURE); 
			} 

			printf("Waiting for a connection ...\n");
			if ((sock = accept(server_fd, (struct sockaddr *)&this->serv_addr,  
							(socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			printf("Connection established ...\n");
		}
		else{
			if ((sock = socket(this->socket_family, this->socket_type, 0)) < 0)
				printf("Socket creation error\n");
			else
				printf("Socket creation successful\n");
			this->establishConnect();
		}
	}
}
/**
 * @brief Destroy the Py C Client object
 * 
 */
EzCppClient::~EzCppClient(){};

/**
 * @brief Connect to the socket explicitly
 * Useful if you want to connect at a point much after
 * object initialization
 */
void EzCppClient::establishConnect()
{
	if (connect(this->sock, (struct sockaddr *)&this->serv_addr, sizeof(this->serv_addr)) < 0)
		printf("\nConnection Failed \n");
	else
		printf("\n Socket connection successful \n");
}

/**
 * @brief Close the connection
 * 
 */
void EzCppClient::Disconnect()
{
	close(sock);
}

// Incoming
/**
 * @brief Read bool value received on port
 * 
 * @return std::pair<bool, bool> Receive (received_flag, received_bool)
 */
std::pair<bool, bool> EzCppClient::readBool()
{
	bool ret = true, value = false;
	try
	{
		std::string received = this->readString();
		if (!received.compare("true") || !received.compare("1")) // If strings match, compare gives 0
			value = true;
		else if (!received.compare("false") || !received.compare("0"))
			;
		else
		{
			ret = false;
			throw(received);
		}
	}
	catch (std::string message)
	{
		printf("\n Message received: %s", message.c_str());
		printf("\n Unable to collect proper boolean information from message.\n");
	}
	return std::pair<bool, bool>(ret, value);
}

/**
 * @brief Read message received on port
 * 
 * @return std::string  Received buffer
 */
std::string EzCppClient::readString()
{
	const int buffer_size = this->readInt();
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);

	if (this->debug)
		printf("readString buffer received: %s\n", buffer);

	std::string str;
	// This cleans up the extra characters that may appear
	// even if str(buffer) is used
	for (int i = 0; i < buffer_size; ++i)
		str.push_back(buffer[i]);
	return str;
}

/**
 * @brief Read integer value received on port
 * 
 * @param buffer_size Size of buffer to be read (in bytes)
 * @return int Received integer
 */
int EzCppClient::readInt(const int buffer_size)
{
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);

	if (this->debug)
		printf("readInt buffer received: %s\n", buffer);

	std::ostringstream s(buffer);
	return std::stoi(s.str().substr(0,buffer_size));
}

/**
 * @brief Read float value received on port
 * 
 * @param buffer_size Size of buffer to be read (in bytes)
 * @return int Received float
 */
float EzCppClient::readFloat(const int buffer_size)
{
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);

	if (this->debug)
		printf("readFloat buffer received: %s\n", buffer);

	std::ostringstream s(buffer);
	return std::stof(s.str());
}

/**
 * @brief Read a list of integer values
 * 
 * @return std::vector<int> Integer List received
 */
std::vector<int> EzCppClient::readIntList()
{
	const int buffer_size = this->readInt(); // get message size
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);

	if (this->debug)
		printf("readIntList buffer received: %s\n", buffer);

	// remove the [] characters around the received list
	std::string str(buffer);
	str = str.substr(1, str.find("]") - 1);

	std::stringstream ss(str);
	std::string ss_elem; // substring
	std::vector<int> v;
	while (getline(ss, ss_elem, ','))
	{
		v.push_back(std::stoi(ss_elem));
	}
	return v;
}

/**
 * @brief Read a list of float values
 * 
 * @return std::vector<int> Float List received
 */
std::vector<float> EzCppClient::readFloatList()
{
	const int buffer_size = this->readInt(); // get message size
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);

	if (this->debug)
		printf("readFloatList buffer received: %s\n", buffer);

	// remove the [] characters around the received list
	std::string str(buffer);
	str = str.substr(1, str.find("]") - 1);

	std::stringstream ss(str);
	std::string ss_elem; // substring
	std::vector<float> v;
	while (getline(ss, ss_elem, ','))
	{
		v.push_back(std::stof(ss_elem));
	}
	return v;
}

/**
 * @brief Read an OpenCV Image
 * 
 * @param receive_size_first Bool flag to get image size from other end
 * @param buffer_size  If bool flag is not set to true, set a buffer size
 * as needed
 * @return cv::Mat Received image 
 */
cv::Mat EzCppClient::readImage()
{
	const int buffer_size = this->readInt();
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);

	std::vector<char> data;
	for (int i = 0; i < buffer_size; i++)
		data.push_back(buffer[i]);

	cv::Mat frame;
	frame = cv::imdecode(cv::Mat(data), 1);

	if (this->debug)
	{
		std::cout << "Received the frame\n";
		cv::imwrite("received.jpg", frame);
	}

	return frame;
}

// Outgoing
/**
 * @brief Send bool value
 * 
 * @param data Bool value to be sent
 */
void EzCppClient::sendBool(bool data)
{
	this->sendString(bool(data) ? "true" : "false");
}

/**
 * @brief Send string
 * 
 * @param msg String to be sent
 */
void EzCppClient::sendString(std::string msg)
{
	const int buffer_size = msg.size();
	this->sendInt(buffer_size);
	const char *msg_ptr = msg.c_str();

	if (this->debug)
		std::cout << "Message sent length : " << strlen(msg_ptr) << "\n";

	send(this->sock, msg_ptr, strlen(msg_ptr), 0);
}

/**
 * @brief Send int
 * 
 * @param data Int to be sent
 */
void EzCppClient::sendInt(int data)
{
	std::string int_str = std::to_string(data);
	std::string int_message =
		std::string(16 - int_str.length(), '0') + int_str;
	send(this->sock, int_message.c_str(), 16, 0);
}

/**
 * @brief Send float
 * 
 * @param data Float value to be sent
 */
void EzCppClient::sendFloat(float data)
{
	std::string float_str = std::to_string(data);
	std::string float_message =
		std::string(16 - float_str.length(), '0') + float_str;
	send(this->sock, float_message.c_str(), 16, 0);
}

/**
 * @brief Send vector of ints
 * 
 * @param data Vector of ints to be sent
 */
void EzCppClient::sendIntList(std::vector<int> data)
{
	std::string int_list;
	int_list += "[";
	for (auto val : data)
	{
		int_list.append(std::to_string(val) + ",");
	}
	// remove the extra comma and add a closing bracket
	int_list.substr(0,int_list.length()-2);
	int_list += "]";
	this->sendString(int_list);
}

/**
 * @brief Send vector of floats
 * 
 * @param data Vector of floats to be sent
 */
void EzCppClient::sendFloatList(std::vector<float> data)
{
	std::string float_list;
	float_list += "[";
	for (auto val : data)
	{
		float_list.append(std::to_string(val) + ",");
	}
	// remove the extra comma and add a closing bracket
	float_list.substr(0,float_list.length()-2);
	float_list += "]";
	this->sendString(float_list);
}

/**
 * @brief Send Image
 * 
 * @param img Image to be sent
 * @param send_size_first Whether size message should be sent first
 */
void EzCppClient::sendImage(cv::Mat img, bool send_size_first)
{
	int pixel_number = img.rows * img.cols / 2;

	std::vector<uchar> buf(pixel_number);
	cv::imencode(".jpg", img, buf);

	if (send_size_first)
	{
		std::string length_str = std::to_string(buf.size());
		std::string length_message =
			std::string(16 - length_str.length(), '0') + length_str;
		send(this->sock, length_message.c_str(), 16, 0);
	}

	send(this->sock, buf.data(), buf.size(), 0);
}