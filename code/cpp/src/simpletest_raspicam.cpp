/**
*/
#include <ctime>
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <sys/time.h>
#include <raspicam/raspicam.h>
using namespace std;

inline uint64_t microSinceEpoc(){
    timeval startTime;
    gettimeofday(&startTime, NULL);
    return startTime.tv_sec*1000000 + startTime.tv_usec;
}

int main ( int argc,char **argv ) {
    raspicam::RaspiCam camera; //Camera object
    //Open camera
    cout<<"Opening camera..."<<endl;
    if ( !camera.open()) {cerr<<"Error opening camera"<<endl;return -1;}
    //wait a while until camera stabilizes

    // set to 1080p
    //1920,1080
    //camera.setWidth(1920);
    //camera.setHeight(1080);
    //camera.setFrameRate(45);
    // set flipped image
    camera.setVerticalFlip(true);
    cout << "fps " << camera.getFrameRate() <<  endl;
    cout << "Height " << camera.getHeight() << " Width " << camera.getWidth() <<endl;
    cout<<"Sleeping for 3 secs"<<endl;

    //allocate memory
    //unsigned char *data=new unsigned char[  camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_IGNORE )];
    cout<<"size " << static_cast<int>(camera.getImageBufferSize ()) << endl;
    unsigned char *data=new unsigned char[camera.getImageBufferSize()];

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    //for(int i=0;i<1;i++)
    for(;;)
    {
        //capture
        uint64_t t1 = microSinceEpoc();
        camera.grab();
        uint64_t t2 = microSinceEpoc();
        data = camera.getImageBufferData();

        //camera.retrieve ( data, raspicam::RASPICAM_FORMAT_IGNORE );//get camera image
        cout << "t1 " << t1 <<std::endl;
        cout << "t2 " << t2 <<std::endl;
        cout << "time elapsed " << t2-t1 << " " << 1000000 / (t2-t1) << " fps"<< std::endl;
    }
    //extract the image in rgb format
    //save
    std::ofstream outFile ( "raspicam_image.ppm",std::ios::binary );
    outFile<<"P6\n"<<camera.getWidth() <<" "<<camera.getHeight() <<" 255\n";
    outFile.write ( ( char* ) data, camera.getImageBufferSize());
    cout<<"Image saved at raspicam_image.ppm"<<endl;
    //free resrources
    delete data;
    return 0;
}
//
