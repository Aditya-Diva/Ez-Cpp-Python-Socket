#include "ezcppsocket.h"

int main(int argc, char const *argv[])
{
	EzCppClient c = EzCppClient();

	// Receiving
	std::cout << "Receiving data...\n";

	auto res = c.readBool();
	std::cout << "Bool 1 : " << res.first << "," << res.second << "\n";

	res = c.readBool();
	std::cout << "Bool 2 : " << res.first << "," << res.second << "\n";

	res = c.readBool();
	std::cout << "Bool 3 : " << res.first << "," << res.second << "\n";

	res = c.readBool();
	std::cout << "Bool 4 : " << res.first << "," << res.second << "\n";

	std::cout << "String : " << c.readString() << "\n";

	std::cout << "Int : " << c.readInt() << "\n";
	std::cout << "Float : " << c.readFloat() << "\n";

	std::cout << "Int List : ";
	std::vector<int> int_list = c.readIntList();
	for (int &list_item : int_list)
	{
		std::cout << list_item << " ";
	}
	std::cout << "\n";

	std::cout << "Float List : ";
	std::vector<float> float_list = c.readFloatList();
	for (auto list_item : float_list)
	{
		std::cout << list_item << " ";
	}
	std::cout << "\n";

	cv::Mat recv_img = c.readImage();
	cv::imshow("Cpp Client Received Image", recv_img);
	std::cout << "Press a key to start sending data to server\n";
	cv::waitKey(0);

	// Sending
	std::cout << "Sending data...\n";

	c.sendBool(true);
	c.sendBool(false);
	c.sendBool(0);
	c.sendBool("wtv");

	c.sendString("Hello there, Dr. Strange");

	c.sendInt(512);
	c.sendFloat(3.14);

	std::vector<int> vi(7, 7);
	c.sendIntList(vi);

	std::vector<float> vf(10, 3.14);
	c.sendFloatList(vf);

	cv::Mat img = cv::imread("lena.jpg");
	c.sendImage(img);

	c.Disconnect();

	return 0;
}