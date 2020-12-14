#include "ezcppsocket.h"

int main(int argc, char const *argv[])
{
	EzCppSocket s = EzCppSocket("127.0.0.1", 10000, 2, 1, false, true, 1, true, 5);

	// Receiving
	std::cout << "Receiving data...\n";

	auto res = s.readBool();
	std::cout << "Bool 1 : " << res.first << "," << res.second << "\n";

	res = s.readBool();
	std::cout << "Bool 2 : " << res.first << "," << res.second << "\n";

	res = s.readBool();
	std::cout << "Bool 3 : " << res.first << "," << res.second << "\n";

	res = s.readBool();
	std::cout << "Bool 4 : " << res.first << "," << res.second << "\n";

	std::cout << "String : " << s.readString() << "\n";

	std::cout << "Int : " << s.readInt() << "\n";
	std::cout << "Float : " << s.readFloat() << "\n";

	std::cout << "Int List : ";
	std::vector<int> int_list = s.readIntList();
	for (int &list_item : int_list)
	{
		std::cout << list_item << " ";
	}
	std::cout << "\n";

	std::cout << "Float List : ";
	std::vector<float> float_list = s.readFloatList();
	for (auto list_item : float_list)
	{
		std::cout << list_item << " ";
	}
	std::cout << "\n";

	cv::Mat img = s.readImage();
	cv::imshow("Cpp Server Received Image", img);
	std::cout << "Press a key to start sending data from Cpp server ...\n";
	cv::waitKey(0);
	cv::destroyAllWindows();

	// Sending
	std::cout << "Sending data...\n";

	s.sendBool(true);
	s.sendBool(false);
	s.sendBool(0);
	s.sendBool("wtv");

	s.sendString("Hello there, Dr. Strange");

	s.sendInt(512);
	s.sendFloat(3.14);

	std::vector<int> vi(7, 7);
	s.sendIntList(vi);

	std::vector<float> vf(10, 3.14);
	s.sendFloatList(vf);

	s.sendImage(img);

	s.Disconnect();

	return 0;
}