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
 * @param client_connection_count // How many clients to permit for connection
 * @param server_mode // If true, acts as server, else client
 * @param reconnect_on_address_busy // Pass a timeout in useconds before reconnecting
 * @param tokens // A start and end token to ensure proper delivery of message
 */
EzCppSocket::EzCppSocket(std::string server_address,
						 int server_port,
						 int socket_family, // AF_INET6
						 int socket_type,
						 bool debug,
						 bool auto_connect,
						 int client_connection_count,
						 bool server_mode,
						 float reconnect_on_address_busy,
						 std::pair<std::string, std::string> tokens)
{
	// Dump values in class
	this->server_address = server_address;
	this->server_port = server_port;
	this->socket_family = socket_family;
	this->socket_type = socket_type;
	this->debug = debug;
	this->reconnect_on_address_busy = reconnect_on_address_busy;
	this->tokens = tokens;

	this->serv_addr.sin_family = this->socket_family;
	this->serv_addr.sin_port = htons(this->server_port);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(this->socket_family, this->server_address.c_str(), &this->serv_addr.sin_addr) <= 0)
		printf("\nInvalid address/ Address not supported \n");

	if (auto_connect)
	{
		printf("Starting ");
		printf((server_mode ? "Server" : "Client"));
		printf("...\n");

		printf("Starting up on %s port %s\n", this->server_address.c_str(), std::to_string(this->server_port).c_str());
		if (server_mode)
		{
			int opt = 1;
			int addrlen = sizeof(this->serv_addr);
			int server_fd;

			// Creating socket file descriptor
			if ((server_fd = socket(this->socket_family, this->socket_type, 0)) == 0)
			{
				perror("Socket creation failed");
				exit(EXIT_FAILURE);
			}
			else
				printf("Socket creation successful\n");

			// Forcefully attaching socket to the port
			if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
						   &opt, sizeof(opt)))
			{
				perror("Setsockopt failed");
				exit(EXIT_FAILURE);
			}
			this->serv_addr.sin_addr.s_addr = INADDR_ANY;

			bool address_free_flag = false;
			while (!address_free_flag)
			{
				// Forcefully attaching socket to the specified port
				if (bind(server_fd, (struct sockaddr *)&this->serv_addr,
						 sizeof(this->serv_addr)) < 0)
				{
					printf("\nServer Binding to Address Failed...\n");
					if (this->reconnect_on_address_busy == 0)
						std::cout << "Please make sure address is free, else use reconnect_on_address_busy argument "
								  << "to keep polling in periodic intervals. If you have just run a server previously "
								  << "there's a good chance the previous server will be down in a couple of seconds."
								  << "Use polling functionality to avoid waiting for address to be free again.";
					this->pollingTimeout();
				}
				else
					address_free_flag = true;
			}

			if (listen(server_fd, 1) < 0) // queue of pending connections -> 1
			{
				perror("listen");
				exit(EXIT_FAILURE);
			}

			printf("Waiting for a connection ...\n");
			if ((sock = accept(server_fd, (struct sockaddr *)&this->serv_addr,
							   (socklen_t *)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			if (this->socket_family == AF_INET) // TODO: Check for IPV6 as well
				printf("Connected IP address: %s:%d\n", inet_ntoa(this->serv_addr.sin_addr), htons(this->serv_addr.sin_port));
			printf("Connection established ...\n");
		}
		else
		{
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
EzCppSocket::~EzCppSocket(){};

/**
 * @brief Connect to the socket explicitly
 * Useful if you want to connect at a point much after
 * object initialization
 */
void EzCppSocket::establishConnect()
{
	bool address_free_flag = false;
	while (!address_free_flag) // TODO: Timeout and check again
	{
		std::cout << "Client is waiting to connect to server...\n";
		if (connect(this->sock, (struct sockaddr *)&this->serv_addr, sizeof(this->serv_addr)) < 0)
		{
			perror("\nClient Connection to Server Failed \n");
			this->pollingTimeout();
		}
		else
		{
			printf("\nClient Socket connection to Server Successful \n");
			address_free_flag = true;
		}
	}
}

/**
 * @brief Where polling is required in periodic intervals, this is 
 * the function that implements the timeout for the same.
 */
void EzCppSocket::pollingTimeout()
{
	if (this->reconnect_on_address_busy > 0)
	{
		printf("Will attempt to reconnect in %lf seconds ...\n", this->reconnect_on_address_busy);
		usleep((unsigned int)(this->reconnect_on_address_busy * 1000000));
	}
	else
	{
		if(this->reconnect_on_address_busy != 0){
			printf("Invalid value passed to reconnect_on_address_busy argument");
		}
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Close the connection
 * 
 */
void EzCppSocket::Disconnect()
{
	close(sock);
}

/**
 * @brief A setter function to add a delay between packet read/write
 * which ensures that it does so properly. It's been observed that 
 * increasing this when images don't come through properly helps.
 * @param microseconds 
 */
void EzCppSocket::setSleepBetweenPackets(unsigned int microseconds)
{
	this->sleep_between_packets = microseconds;
}

/**
 * @brief A getter function to get delay between packet read/write
 * 
 * @param microseconds Number of microseconds to wait before read/write of
 * next packet
 */
unsigned int EzCppSocket::getSleepBetweenPackets()
{
	return this->sleep_between_packets;
}

/**
 * @brief A setter function to set size of packets during read/write
 * Note: The value passed should not be more than 65535 (64K)
 * @param number_of_bytes
 */
void EzCppSocket::setPacketSize(unsigned int number_of_bytes)
{
	if (number_of_bytes > 0)
		this->packet_size = number_of_bytes;
	else
		printf("\nInvalid packet size was provided. Not updating packet size.\n");
	
}

/**
 * @brief Getter function to get loop status
 * 
 * @return true Loop is still active
 * @return false Loop has stopped
 */
bool EzCppSocket::getLoopFlag(){
	return this->loop_flag;
}

/**
 * @brief A decorator functionality which prints IPS(iterations per second)
 * 
 * @param func_ptr Pointer to a function
 * @param show_ips Bool flag to display IPS (iterations per second)
 */
void EzCppSocket::loop_func_decorator(void (*func_ptr)(EzCppSocket&), bool show_ips){
	this->loop_iteration_count += 1;
	func_ptr(*this);
	if (this->debug || show_ips){
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - this->loop_start_time); 
		if (duration.count() != 0)
			std::cout << "IPS : " <<  float(this->loop_iteration_count) / float(duration.count()) << "\n";
	}
}

/**
 * @brief Loop to run server
 * 
 * @param func_ptr Pointer to function that should be part of the server loop
 * @param loop_count 
 * Looping behaviour:
 *   if -1, loops until stop_loop is called on server side,
 *   if 0, loops until stop_loop is called from client side (Status string is sent)
 *   else, loops for as many iterations as specified]. Defaults to 0.
 * @param show_ips Bool flag to display IPS (iterations per second)
 */
void EzCppSocket::serverLoop(void (*func_ptr)(EzCppSocket&), int loop_count, bool show_ips){
	this->loop_flag = true;
	this->loop_start_time = std::chrono::high_resolution_clock::now();

	if (loop_count == -1){
		while (this->loop_flag)
			loop_func_decorator(func_ptr, show_ips);
	}

	else if (loop_count == 0){
		// Server is up until Client has gotten its request
		std::string status = "Active";
		while (status.compare("Stop") != 0){
			loop_func_decorator(func_ptr, show_ips);
			status = this->readString();
		}
	}
	else{
		// Server serves for certain iterations
		for(int i = 0; i < loop_count; ++i)
			loop_func_decorator(func_ptr, show_ips);
	}

	this->resetLoop();
}

/**
 * @brief Loop to run client
 * 
 * @param func_ptr Pointer to the function that should be part of client loop
 * @param loop_count 
 * Looping behaviour
 *	if 0, loops until stop_loop is called from client side
 *	else, loops for as many iterations as specified]. Defaults to 0.
 * @param show_ips Bool flag to display IPS (iterations per second)
 */
void EzCppSocket::clientLoop(void (*func_ptr)(EzCppSocket&), int loop_count, bool show_ips){
	this->loop_flag = true;
	this->loop_start_time = std::chrono::high_resolution_clock::now();

	if (loop_count == 0){
		// Server is up until Client has gotten its request
		std::string status = "Active";
		while (status.compare("Stop") != 0){
			loop_func_decorator(func_ptr, show_ips);
			// Set & Send status
			status = (this->loop_flag)?"Active":"Stop";
			this->sendString(status);
		}
	}
	else{
		// Client runs for certain iterations
		for (int i = 0; i < loop_count; i++)
			loop_func_decorator(func_ptr, show_ips);
	}

	this->resetLoop();
}

/**
 * @brief Set flag to stop looping
 * 
 */
void EzCppSocket::stopLoop(){
	this->loop_flag = false;
}

/**
 * @brief Resets all the loop related flags
 * 
 */
void EzCppSocket::resetLoop(){
	this->loop_flag = false;
	this->loop_iteration_count = 0;
	this->loop_start_time = std::chrono::high_resolution_clock::now();
}

/**
 * @brief Insert tokens to the message that is being passed. This includes
 * both the start token at the beginning of the message and the end token
 * at the end of the message.
 * @param msg
 */
void EzCppSocket::insertTokens(std::string &msg)
{
	msg = this->tokens.first + msg + this->tokens.second;
}

/**
 * @brief Extracting tokens from the received messages to get the actual message.
 * This also serves as a check on the validity of the message. Currently throws
 * an error if invalid message is received.
 * @param msg 
 */
void EzCppSocket::extractTokens(std::string &msg)
{
	try
	{
		// Start token extraction
		if (msg.find(this->tokens.first) == 0)
		{
			msg = msg.substr(this->tokens.first.length());
		}
		else
		{
			std::cout << "Starting token was not found at the beginning of message received!"
					  << " Please check if the right kind of data is being sent/received or that"
					  << " the same tokens are set on server and client ends...\n";
			throw "Starting token check in received message failed";
		}

		// End token extraction
		if (msg.rfind(this->tokens.second) == (msg.length() - this->tokens.second.length()))
		{
			msg = msg.substr(0, msg.length() - this->tokens.second.length());
		}
		else
		{
			std::cout << "Ending token was not found at the end of message received!"
					  << " Please check if the right kind of data is being sent/received or that"
					  << " the same tokens are set on server and client ends...\n"
					  << "Additionally, try increasing setSleepBetweenPackets value.\n"
					  << "Current sleep_between_packets value: " << this->getSleepBetweenPackets() << "\n";
			throw "Ending token check in received message failed";
		}
	}
	catch (const char *msg)
	{
		perror(msg);
	}
}

// Incoming

/**
 * @brief Read bool value received on port
 * 
 * @return std::pair<bool, bool> Receive (received_flag, received_bool)
 */
std::pair<bool, bool> EzCppSocket::readBool()
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
std::string EzCppSocket::readString()
{
	const int buffer_size = this->readInt();
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);

	std::string str(&buffer[0], &buffer[buffer_size]);
	this->extractTokens(str);

	if (this->debug)
	{
		printf("readString buffer received: %s\n", buffer);
		printf("Final string : %s\n", str.c_str());
	}

	return str;
}

/**
 * @brief Read integer value received on port
 * 
 * @param buffer_size Size of buffer to be read (in bytes)
 * @return int Received integer
 */
int EzCppSocket::readInt(const int buffer_size)
{
	const int token_compensated_buffer_size = this->tokens.first.length() + buffer_size + this->tokens.second.length();
	char buffer[token_compensated_buffer_size] = {0};
	int valread = read(sock, buffer, token_compensated_buffer_size);

	std::string str(&buffer[0], &buffer[token_compensated_buffer_size]);
	this->extractTokens(str);

	if (this->debug)
	{
		printf("readInt buffer received: %s\n", buffer);
		printf("Converted int : %i\n", std::stoi(str));
	}

	return std::stoi(str);
}

/**
 * @brief Read float value received on port
 * 
 * @param buffer_size Size of buffer to be read (in bytes)
 * @return int Received float
 */
float EzCppSocket::readFloat(const int buffer_size)
{
	const int token_compensated_buffer_size = this->tokens.first.length() + buffer_size + this->tokens.second.length();
	char buffer[token_compensated_buffer_size] = {0};
	int valread = read(sock, buffer, token_compensated_buffer_size);

	std::string str(&buffer[0], &buffer[token_compensated_buffer_size]);
	this->extractTokens(str);

	if (this->debug)
	{
		printf("readFloat buffer received: %s\n", buffer);
		printf("Converted float : %f\n", std::stof(str));
	}

	return std::stof(str);
}

/**
 * @brief Read a list of integer values
 * 
 * @return std::vector<int> Integer List received
 */
std::vector<int> EzCppSocket::readIntList()
{
	const int buffer_size = this->readInt(); // get message size
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);

	// remove the [] characters around the received list
	std::string str(&buffer[0], &buffer[buffer_size]);
	this->extractTokens(str);
	str = str.substr(1, str.find("]") - 1);

	std::stringstream ss(str);
	std::string ss_elem; // substring
	std::vector<int> v;
	while (getline(ss, ss_elem, ','))
	{
		v.push_back(std::stoi(ss_elem));
	}

	if (this->debug)
	{
		printf("readIntList buffer received: %s\n", buffer);
		for (auto elem : v)
		{
			printf("%i ,", elem);
		}
		std::cout << "\n";
	}

	return v;
}

/**
 * @brief Read a list of float values
 * 
 * @return std::vector<int> Float List received
 */
std::vector<float> EzCppSocket::readFloatList()
{
	const int buffer_size = this->readInt(); // get message size
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);

	// remove the [] characters around the received list
	std::string str(&buffer[0], &buffer[buffer_size]);
	this->extractTokens(str);
	str = str.substr(1, str.find("]") - 1);

	std::stringstream ss(str);
	std::string ss_elem; // substring
	std::vector<float> v;
	while (getline(ss, ss_elem, ','))
	{
		v.push_back(std::stof(ss_elem));
	}

	if (this->debug)
	{
		printf("readFloatList buffer received: %s\n", buffer);
		for (auto elem : v)
		{
			printf("%f ,", elem);
		}
		std::cout << "\n";
	}
	return v;
}

/**
 * @brief Read an OpenCV Image
 * 
 * @return cv::Mat Received Image
 */
cv::Mat EzCppSocket::readImage()
{
	const int complete_buffer_size = this->readInt();
	std::vector<uchar> data;
	data.clear();

	unsigned packet_start_index = 0;
	unsigned int packet_size_curr = this->packet_size;
	uchar buffer[this->packet_size] = {0};
	// Read packets of size defined by packet_size
	while (packet_start_index < complete_buffer_size)
	{
		if ((packet_start_index + this->packet_size) > complete_buffer_size)
			packet_size_curr = complete_buffer_size - packet_start_index;

		int valread = read(sock, buffer, packet_size_curr);

		for (int i = 0; i < packet_size_curr; i++)
			data.push_back(buffer[i]);

		if (this->debug)
		{
			std::cout << "\nReceiving packet no. " << packet_start_index / this->packet_size << "\n";
			std::cout << "This packet is of size : " << packet_size_curr << "\n";
			std::cout << "Current size of data accumulated : " << data.size() << "\n";
		}
		packet_start_index += packet_size_curr;
		usleep(this->sleep_between_packets);
	}

	std::string data_str(data.begin(), data.end());
	this->extractTokens(data_str);
	std::vector<char> img_data(data_str.begin(), data_str.end());

	cv::Mat frame;
	frame = cv::imdecode(cv::Mat(img_data), 1);

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
void EzCppSocket::sendBool(bool data)
{
	this->sendString(bool(data) ? "true" : "false");
}

/**
 * @brief Send string
 * 
 * @param msg String to be sent
 */
void EzCppSocket::sendString(std::string msg)
{
	this->insertTokens(msg);
	const int buffer_size = msg.size();
	this->sendInt(buffer_size);
	const char *msg_ptr = msg.c_str();

	if (this->debug)
	{
		std::cout << "Message sent length : " << strlen(msg_ptr) << "\n";
		std::cout << "Sending message : " << msg << "\n";
	}

	send(this->sock, msg_ptr, strlen(msg_ptr), 0);
}

/**
 * @brief Send int
 * 
 * @param data Int to be sent
 */
void EzCppSocket::sendInt(int data)
{
	std::string int_str = std::to_string(data);
	std::string int_message =
		std::string(16 - int_str.length(), '0') + int_str;
	this->insertTokens(int_message);

	if (this->debug)
	{
		std::cout << "Message sent length : " << int_message.length() << "\n";
		std::cout << "Sending message : " << int_message << "\n";
	}

	send(this->sock, int_message.c_str(), this->tokens.first.length() + 16 + this->tokens.second.length(), 0);
}

/**
 * @brief Send float
 * 
 * @param data Float value to be sent
 */
void EzCppSocket::sendFloat(float data)
{
	std::string float_str = std::to_string(data);
	std::string float_message =
		std::string(16 - float_str.length(), '0') + float_str;
	this->insertTokens(float_message);

	if (this->debug)
	{
		std::cout << "Message sent length : " << float_message.length() << "\n";
		std::cout << "Sending message : " << float_message << "\n";
	}

	send(this->sock, float_message.c_str(), this->tokens.first.length() + 16 + this->tokens.second.length(), 0);
}

/**
 * @brief Send vector of ints
 * 
 * @param data Vector of ints to be sent
 */
void EzCppSocket::sendIntList(std::vector<int> data)
{
	std::string int_list;
	int_list += "[";
	for (auto val : data)
		int_list.append(std::to_string(val) + ",");

	// remove the extra comma and add a closing bracket
	int_list.substr(0, int_list.length() - 2);
	int_list += "]";
	this->sendString(int_list);
}

/**
 * @brief Send vector of floats
 * 
 * @param data Vector of floats to be sent
 */
void EzCppSocket::sendFloatList(std::vector<float> data)
{
	std::string float_list;
	float_list += "[";
	for (auto val : data)
	{
		float_list.append(std::to_string(val) + ",");
	}
	// remove the extra comma and add a closing bracket
	float_list.substr(0, float_list.length() - 2);
	float_list += "]";
	this->sendString(float_list);
}

/**
 * @brief Send Image
 * 
 * @param img Image to be sent
 */
void EzCppSocket::sendImage(cv::Mat img)
{
	int pixel_number = img.rows * img.cols / 2;

	std::vector<uchar> buf(pixel_number);
	cv::imencode(".jpg", img, buf);

	// Token handling
	std::string buf_str(buf.begin(), buf.end());
	this->insertTokens(buf_str);
	buf = std::vector<uchar>(buf_str.begin(), buf_str.end());

	// Send image size first
	if (this->debug)
		std::cout << "Total image buffer size:" << sizeof(buf) << "\n";
	this->sendInt(buf.size());

	unsigned packet_start_index = 0;
	unsigned int packet_size_curr = this->packet_size;
	// Break into packets of size defined by packet_size
	while (packet_start_index < buf.size())
	{
		if ((packet_start_index + this->packet_size) > buf.size())
			packet_size_curr = buf.size() - packet_start_index;

		std::vector<uchar> buf_packet(&buf[packet_start_index], &buf[packet_start_index + packet_size_curr]);
		if (this->debug)
		{
			std::cout << "\nSending packet no. " << packet_start_index / this->packet_size << "\n";
			std::cout << "This packet is of size : " << buf_packet.size() << "\n";
		}
		send(this->sock, buf_packet.data(), buf_packet.size(), 0);
		packet_start_index += packet_size_curr;
		usleep(this->sleep_between_packets);
	}
}