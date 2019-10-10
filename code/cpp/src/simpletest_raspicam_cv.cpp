#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>

#include <stdint.h>

#include <opencv2/opencv.hpp>

using namespace std;

inline uint64_t microSinceEpoc(){
    // Get time stamp in microseconds.
{
    uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    return us;
}
}


int main ( int argc,char **argv ) {

    time_t timer_begin,timer_end;
    raspicam::RaspiCam_Cv camera;
    cv::Mat image;
    int nCount=0;
    //set camera params
    camera.set( cv::CAP_PROP_FORMAT, CV_8UC1 );



    //camera.set(cv::CAP_PROP_FRAME_WIDTH,  1920);
    //camera.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    //camera.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));

    //camera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    //camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    //camera.set(cv::CAP_PROP_FRAME_WIDTH, 480);
    //camera.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
    //camera.set(cv::CAP_PROP_FPS, 60);

    camera.set(cv::CAP_PROP_BUFFERSIZE, 3);
    //cv::CAP

    //Open camera
    cout<<"Opening camera..."<<endl;
    if (!camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    //Start capture
    cout<<"Capturing "<<nCount<<" frames ...."<<endl;
    time ( &timer_begin );
    //for ( int i=0; i<nCount; i++ )

    uint64_t last=0;
    for(int i=0;i<50;i++)
    {
        camera.grab();
        camera.retrieve (image);
        nCount++;

    }

    time ( &timer_end ); /* get current time; same as: timer = time(NULL)  */
    double secondsElapsed = difftime ( timer_end,timer_begin );
    cout<< secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;


    for(;;)
    {
        uint64_t t1 = microSinceEpoc();
        camera.grab();
        uint64_t t2 = microSinceEpoc();
        camera.retrieve (image);
        uint64_t t3 = microSinceEpoc();
        uint64_t t4 = camera.getFrameCallbackTime();

        //std::cout << "t1 " << t1 << endl;
        //std::cout << "t2 " << t2 << endl;
        //std::cout << "t3 " << t3 << endl;
        //std::cout << "t4 " << t4 << endl;
        std::cout << "t2-t1 " << t2-t1 << endl;
        std::cout << "t3-t1 " << t3-t1 << endl;
        std::cout << "t4-t1 " << t4-t1 << endl;
        //std::cout << "gap1 " << t2-t4 << endl;
        //std::cout << "gap2 " << t3-t4 << endl;

        //if ( i%5==0 )  cout<<"\r captured "<<i<<" images"<<std::flush;
        //cv::Mat dst;
        //cv::resize(image, dst, cv::Size(640,480));

        uint64_t mics = microSinceEpoc();
   	    //std::cout << (mics - last) << " " << (1000000 / (mics - last)) << " fps" << std::endl;
   	    last= mics;

        //cv::imshow("edges", dst);
        //if(cv::waitKey(30) >= 27) break;


    }
    cout<<"Stop camera..."<<endl;
    camera.release();

    cout<< secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;

    //show time statistics
    //save image
    //cv::imwrite("raspicam_cv_image.jpg",image);

    std::cout << "buffersize" << camera.get(cv::CAP_PROP_BUFFERSIZE) << std::endl;
    std::cout << "rows " <<image.rows << " cols " <<image.cols << " type " << image.type() <<" cont " <<image.isContinuous() <<" channels " <<image.channels() << std::endl;

    cout<<"Image saved at raspicam_cv_image.jpg"<<endl;
}
