#include "ezcppsocket.h"

int main(int argc, char const *argv[])
{
	EzCppSocket c = EzCppSocket("127.0.0.1", 10000, 2, 1, false, true, 1, false);

	// Sending
	std::cout << "Sending image...\n";
	cv::Mat img = cv::imread("../resources/lena.jpg");
	c.sendImage(img);

	// Receiving
	std::cout << "Receiving data...\n";
	cv::Mat recv_img = c.readImage();

	return 0;
}