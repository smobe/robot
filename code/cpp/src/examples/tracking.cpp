#include <algorithm>
#include <iostream>
#include <random>
#include <boost/circular_buffer.hpp>
#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>

#include <utils.h>

using namespace std;

/**
Simple example of creating a camera and streaming images
**/
static void on_trackbar( int, void* )
{
}

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
    cv::Mat image, hsv_image, mask, mask_final;
    //std::vector<uint8_t> lowerb = {29, 86, 6};
    //std::vector<uint8_t> upperb = {64, 255, 255};
    //std::vector<uint8_t> lowerb = {39, 186, 6};
    //std::vector<uint8_t> upperb = {54, 186, 255};

    int lower_h=31, lower_s=56, lower_v=21;
    int upper_h=85, upper_s=198, upper_v=98;

    std::mt19937 rng;
    std::uniform_int_distribution<int> row_dist(0, camera.get(cv::CAP_PROP_FRAME_HEIGHT));
    std::uniform_int_distribution<int> col_dist(0, camera.get(cv::CAP_PROP_FRAME_WIDTH));
    boost::circular_buffer<std::pair<int,int> > trail(30);

    for(;;)
    {
        camera.grab();
        camera.retrieve (image);

        cvtColor(image, hsv_image, cv::COLOR_BGR2HSV);

        std::vector<int> lowerb = {lower_h, lower_s, lower_v};
        std::vector<int> upperb = {upper_h, upper_s, upper_v};

        cv::inRange(hsv_image, lowerb, upperb, mask);
        //cv::dilate(mask, mask_final, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);




        cv::namedWindow("Linear Blend", cv::WINDOW_AUTOSIZE); // Create Window

        cv::createTrackbar( "Lower h", "Linear Blend", &lower_h, 255 );
        cv::createTrackbar( "Upper h", "Linear Blend", &upper_h, 255 );

        cv::createTrackbar( "Lower s", "Linear Blend", &lower_s, 255 );
        cv::createTrackbar( "Upper s", "Linear Blend", &upper_s, 255 );

        cv::createTrackbar( "Lower v", "Linear Blend", &lower_v, 255 );
        cv::createTrackbar( "Upper v", "Linear Blend", &upper_v, 255 );

        cv::imshow("mask", mask);

        int cnt = 0;
        int row, col;
        double total_row=0, total_col=0;
        int n_samples = 10;
        int tries = 0;
        while (cnt < n_samples){
            row = row_dist(rng);
            col = col_dist(rng);

            if (mask.at<uchar>(row, col) > 0){
                total_row += row;
                total_col += col;
                cnt++;
            }
            tries ++;
        }
        total_row /= n_samples;
        total_col /= n_samples;
        cout << tries << " " << total_row << " " << total_col << endl;

        // store latest value
        trail.push_back(std::pair<int,int>(total_col, total_row));
        double psize = 0;
        double step = 20 / trail.size();

        for (std::pair<int,int>& p : trail){
            cv::circle(image, cv::Point2i(p.first,p.second), 3, cv::Scalar(0,125,230), 4, 3);
            psize += step;
        }

        cv::imshow("edges", image);
        if(cv::waitKey(30) >= 27) break;
    }

    cout<<"Stop camera..."<<endl;
    camera.release();

    std::cout << "buffersize" << camera.get(cv::CAP_PROP_BUFFERSIZE) << std::endl;
    std::cout << "rows " <<image.rows << " cols " <<image.cols << " type " << image.type() <<" cont " <<image.isContinuous() <<" channels " <<image.channels() << std::endl;

    cout<<"Image saved at raspicam_cv_image.jpg"<<endl;
}
