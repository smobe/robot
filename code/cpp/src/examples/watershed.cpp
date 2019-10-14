
#include <iostream>


#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>

#include <utils.h>

using namespace std;
using namespace cv;

/**
Simple example of creating a camera and streaming images
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

    vector<cv::Vec3b> colorTab;
    for(int i = 0; i < 50; i++ )
    {
        int b = cv::theRNG().uniform(0, 255);
        int g = cv::theRNG().uniform(0, 255);
        int r = cv::theRNG().uniform(0, 255);
        colorTab.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
    }

    //Start capture
    cv::Mat image;
    Mat markerMask,imgGray;
    for(;;)
    {
        int compCount=0;

        camera.grab();
        camera.retrieve (image);

        cv::imshow("edges", image);

        cv::cvtColor(image, markerMask, cv::COLOR_BGR2GRAY);
        cv::cvtColor(markerMask, imgGray, cv::COLOR_GRAY2BGR);
        //markerMask = cv::Scalar::all(0);


        cv::imshow("edges", image);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(markerMask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
        if( contours.empty() )
            continue;

        Mat markers(markerMask.size(), CV_32S);
        markers = Scalar::all(0);

        int idx = 0;
        for( ; idx >= 0; idx = hierarchy[idx][0], compCount++ )
            drawContours(markers, contours, idx, Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);
        if( compCount == 0 )
            continue;
        cout << "comp count " << compCount << endl;



        cv::watershed( image, markers );


        //cv::Mat markers(image.size(), CV_32S);
        cv::Mat wshed(markers.size(), CV_8UC3);
            // paint the watershed image
        for(int i = 0; i < markers.rows; i++ )
        {
                for(int j = 0; j < markers.cols; j++ )
                {
                    int index = markers.at<int>(i,j);
                    if( index == -1 )
                        wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(255,255,255);
                    else if( index <= 0 || index > compCount )
                        wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
                    else
                        wshed.at<cv::Vec3b>(i,j) = colorTab[index - 1];
                }
        }

        //wshed = wshed*0.5 + image_grey*0.5;
        cv::imshow( "watershed transform", wshed );



        if(cv::waitKey(30) >= 27) break;
    }

    cout<<"Stop camera..."<<endl;
    camera.release();

    std::cout << "buffersize" << camera.get(cv::CAP_PROP_BUFFERSIZE) << std::endl;
    std::cout << "rows " <<image.rows << " cols " <<image.cols << " type " << image.type() <<" cont " <<image.isContinuous() <<" channels " <<image.channels() << std::endl;

    cout<<"Image saved at raspicam_cv_image.jpg"<<endl;
}
