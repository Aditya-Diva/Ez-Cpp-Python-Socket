#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#define PORT 10000

void SendImage(cv::Mat img, int sock)
{
    int pixel_number = img.rows * img.cols / 2;

    std::cout << "Image rows:" << img.rows << "\n";
    std::cout << "Image cols:" << img.cols << "\n";
    std::cout << "No. of bytes expected: " << img.rows* img.cols * 3 << "\n";

    std::vector<uchar> buf(pixel_number);
    cv::imencode(".jpg", img, buf);

    int length = buf.size();
    std::string length_str = std::to_string(length);
    std::cout << "Length of string:" << length_str << "\n";
    std::string message_length =
        std::string(16 - length_str.length(), '0') + length_str;

    send(sock, message_length.c_str(), 16, 0);
    send(sock, buf.data(), length, 0);
}

int main(int argc, char const *argv[]) 
{ 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char *hello = "Hello from client"; 
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 
    else
    {
        printf("\n Socket creation successful \n");
    }
    

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
    else
    {
        printf("\n Socket connection successful \n");
    }

    cv::Mat image = cv::imread("lena.jpg");
    SendImage(image,sock);
    printf("Image sent...");

	// send(sock , hello , strlen(hello) , 0 ); 
	// printf("Hello message sent\n"); 
	
    valread = read( sock , buffer, 1024); 
	printf("%s\n",buffer ); 
	return 0; 
} 
