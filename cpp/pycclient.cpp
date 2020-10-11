#include "pycclient.h"

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
PyCClient::PyCClient(std::string server_address,
					 int server_port,
					 int socket_family, // AF_INET6
					 int socket_type,
					 bool debug,
					 bool auto_connect)
{
	// Dump values in class
	this->server_address = server_address;
	this->server_port = server_port;
	this->socket_family = socket_family;
	this->socket_type = socket_type;
	this->debug = debug;

	if ((sock = socket(this->socket_family, this->socket_type, 0)) < 0)
		printf("Socket creation error\n");
	else
		printf("Socket creation successful\n");

	this->serv_addr.sin_family = this->socket_family;
	this->serv_addr.sin_port = htons(this->server_port);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(this->socket_family, this->server_address.c_str(), &this->serv_addr.sin_addr) <= 0)
		printf("\nInvalid address/ Address not supported \n");

	if (auto_connect)
	{
		this->establishConnect();
	}
}
/**
 * @brief Destroy the Py C Client object
 * 
 */
PyCClient::~PyCClient(){};

/**
 * @brief Connect to the socket explicitly
 * Useful if you want to connect at a point much after
 * object initialization
 */
void PyCClient::establishConnect()
{
	if (connect(this->sock, (struct sockaddr *)&this->serv_addr, sizeof(this->serv_addr)) < 0)
		printf("\nConnection Failed \n");
	else
		printf("\n Socket connection successful \n");
}

// Incoming

/**
 * @brief Read message received on port
 * 
 * @return std::string  Received buffer
 */
std::string PyCClient::readString()
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
int PyCClient::readInt(const int buffer_size)
{
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);
	std::ostringstream s(buffer);
	return std::stoi(s.str());
}

/**
 * @brief Read a list of integer values
 * 
 * @return std::vector<int> Integer List received
 */
std::vector<int> PyCClient::readIntList()
{
	const int buffer_size = this->readInt(); // get message size
	char buffer[buffer_size] = {0};
	int valread = read(sock, buffer, buffer_size);

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
 * @brief Read an OpenCV Image
 * 
 * @param receive_size_first Bool flag to get image size from other end
 * @param buffer_size  If bool flag is not set to true, set a buffer size
 * as needed
 * @return cv::Mat Received image 
 */
cv::Mat PyCClient::readImage()
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

void PyCClient::sendString(std::string msg)
{
	const int buffer_size = msg.size();
	this->sendInt(buffer_size);
	const char *msg_ptr = msg.c_str();

	if (this->debug)
		std::cout << "Message sent length : " << strlen(msg_ptr) << "\n";

	send(this->sock, msg_ptr, strlen(msg_ptr), 0);
}

void PyCClient::sendInt(int data)
{
	std::string int_str = std::to_string(data);
	std::string int_message =
		std::string(16 - int_str.length(), '0') + int_str;
	send(this->sock, int_message.c_str(), 16, 0);
}

/**
 * @brief Send Image
 * 
 * @param img Image to be sent
 * @param send_size_first Whether size message should be sent first
 */
void PyCClient::sendImage(cv::Mat img, bool send_size_first)
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