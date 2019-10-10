#include "opencv2/opencv.hpp"

using namespace cv;

#include <chrono>

#include <iostream>
#include <stdint.h>
#include <sys/time.h>


// ...
#include <stdint.h>
#include <sys/time.h>
inline uint64_t microSinceEpoc(){
    timeval startTime;
    gettimeofday(&startTime, NULL);
    return startTime.tv_sec*1000000 + startTime.tv_usec;
}


#include "opencv.h"

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));

    if (0) {
        cap.set(cv::CAP_PROP_FRAME_WIDTH,  1920);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    //    cap.set(cv::CAP_PROP_FPS, 60);
    }else{
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(cv::CAP_PROP_FPS, 60);
    }

    Mat edges;
    namedWindow("edges",1);
    uint64_t last = 0;
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        uint64_t mics = microSinceEpoc();
   	    std::cout << (mics - last) << " " << (1000000 / (mics - last)) << " fps" << std::endl;
   	    std::cout << "rows " <<frame.rows << " cols " <<frame.cols << " type " << frame.type() <<" cont " <<frame.isContinuous() <<" channels " <<frame.channels();

	    last = mics;
        //cv::flip(frame, frame, 1);
        //cvtColor(frame, edges, COLOR_BGR2GRAY);
        //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
        imshow("edges", frame);
        if(waitKey(30) >= 27) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
