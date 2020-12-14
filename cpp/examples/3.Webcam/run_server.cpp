#include "ezcppsocket.h"
#include <opencv2/imgproc/imgproc.hpp>

int MODE = 0;

void server_operation(EzCppSocket &s){
    cv::Mat frame, edges;

    //Receive image from client
    frame = s.readImage();
    if (!frame.empty())
    {
        // Processing here
        cv::cvtColor(frame, edges, cv::COLOR_BGR2GRAY);
        cv::Canny(edges, edges, 100, 200, 3);
    }

    // Send processed image to client
    s.sendImage(edges);
}


int main()
{

    EzCppSocket s = EzCppSocket("127.0.0.1", 10000, 2, 1, false, true, 1, true, 5, std::pair<std::string, std::string>("start", "end"));
    if (MODE == 0)
        s.serverLoop(&server_operation);

    return 0;
}