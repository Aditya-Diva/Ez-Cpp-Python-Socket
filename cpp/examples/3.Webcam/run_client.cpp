#include "ezcppsocket.h"
#include <opencv2/imgproc/imgproc.hpp>

int MODE = 0;

void client_operation(EzCppSocket &c){
    static cv::VideoCapture cap(0);
    static cv::Mat frame, result;
    static bool first_run = true;
    if (first_run){
        cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);
        first_run = false;
    }

    if (!cap.isOpened()) {
        std::cerr << "ERROR: Could not open camera\n";
        exit(1);
    }

    if (c.getLoopFlag() == false){
        cap.release();
        cv::destroyAllWindows();
    }

    cap >> frame;
    cv::resize(frame, frame, cv::Size(1920, 1080));
    if (!frame.empty())
    {
        // Send image to server
        c.sendImage(frame);

        // Receive image from server
        result = c.readImage();

        cv::imshow("frame", result);
        if (cv::waitKey(1) == 27){
            c.stopLoop();
        }
    }
}

int main(int argc, char **argv)
{
    EzCppSocket c = EzCppSocket("127.0.0.1", 10000, 2, 1, false, true, 1, false, 5, std::pair<std::string, std::string>("start", "end"));
    std::cout << "Press Esc to exit !\n";

    // Set configuration as needed (Increase this value incase end token is
    // not being read, i.e. give time for packets to be written properly from
    // server end
    c.setSleepBetweenPackets(100); 

    cv::Mat frame, result;
    if (MODE==0)
        c.clientLoop(&client_operation);

    return 0;
}