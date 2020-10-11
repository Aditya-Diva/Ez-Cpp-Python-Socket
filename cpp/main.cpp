#include "pycclient.h"

int main(int argc, char const *argv[])
{
	PyCClient c = PyCClient();

	// Sending

	c.sendInt(512);

	c.sendString("It was the only way.");

	cv::Mat img = cv::imread("lena.jpg");
	c.sendImage(img);

	// Receiving
	std::cout << c.readString() << "\n";

	std::cout << c.readInt() << "\n";

	std::vector<int> list = c.readIntList();
	for(int &list_item: list){
		std::cout << list_item << " "; 
	}
	std::cout << "\n";

	cv::Mat recv_img = c.readImage();
	cv::imshow("Cpp Received Image", recv_img);
	cv::waitKey(0);

	return 0;
}