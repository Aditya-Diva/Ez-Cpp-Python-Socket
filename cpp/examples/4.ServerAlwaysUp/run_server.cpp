#include "ezcppsocket.h"

int main(int argc, char const *argv[])
{
	while (true){
		EzCppSocket s = EzCppSocket("127.0.0.1", 10000, 2, 1, false, true, 1, true, 2);

		// Receiving
		std::cout << "Receiving data...\n";
		cv::Mat recv_img = s.readImage();

		// Sending
		cv::Mat img = cv::imread("lena.jpg");
		s.sendImage(img);
	}

	return 0;
}