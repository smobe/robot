
#include <iostream>


#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>

#include <utils.h>

using namespace std;

/**
Simple example of creating a camera and streaming images

The streamed images also have a 2D filter that calculated the difference between a pixel and its

**/


int main ( int argc,char **argv ) {

    time_t timer_begin,timer_end;
    raspicam::RaspiCam_Cv camera;

    //set camera params
    //camera.set( cv::CAP_PROP_FORMAT, CV_8UC1 );

    //camera.set(cv::CAP_PROP_FRAME_WIDTH,  1280);
    //camera.set(cv::CAP_PROP_FRAME_HEIGHT, 960);

    //camera.set(cv::CAP_PROP_FRAME_WIDTH,  1920);
    //camera.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    camera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    //camera.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));

    bool res = camera.set(cv::CAP_PROP_FPS, 30);
    cout << "res: " << res << " " << "fps: " << camera.get(cv::CAP_PROP_FPS) <<endl;

    //Open camera
    cout<<"Opening camera..."<<endl;
    if (!camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}

    //Start capture
    cv::Mat image, filtered, gfiltered;
    cv::Mat kernel;

    cv::Point anchor( -1, -1 );
    double delta = 0;
    int ddepth = -1;

//    kernel = cv::Mat::ones( 3, 3, CV_32F )/(float)(3*3);
    /*
    const double v = -1.0/8.0;
    float vals[] =  {v,v,v,v,1,v,v,v,v};
    kernel = cv::Mat( 3, 3, CV_32F, vals);
    */

    const double v = -1.0/24.0;
    float vals[] =  {v,v,v,v,v,
                     v,v,v,v,v,
                     v,v,1,v,v,
                     v,v,v,v,v,
                     v,v,v,v,v };
    kernel = cv::Mat( 5, 5, CV_32F, vals);


    for(;;)
    {
        camera.grab();
        camera.retrieve (image);
        cv::imshow("edges", image);

        /// Apply filter
        filter2D(image, filtered, ddepth , kernel, anchor, delta, cv::BORDER_DEFAULT );

        cvtColor(filtered, gfiltered, CV_8UC1);
        imshow( "filter", gfiltered );

        if(cv::waitKey(30) >= 27) break;
    }

    cout<<"Stop camera..."<<endl;
    camera.release();

    std::cout << "buffersize" << camera.get(cv::CAP_PROP_BUFFERSIZE) << std::endl;
    std::cout << "rows " <<image.rows << " cols " <<image.cols << " type " << image.type() <<" cont " <<image.isContinuous() <<" channels " <<image.channels() << std::endl;

    cout<<"Image saved at raspicam_cv_image.jpg"<<endl;
}
