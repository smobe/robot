#include <iostream>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <boost/thread.hpp>

#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>

#include <utils.h>

using namespace std;

std::atomic_bool run_camera(true);

#define PORT 8888
//#define PI_SERVER_IP "192.168.1.200"
#define PI_SERVER_IP "127.0.0.1"
//#define PI_SERVER_IP "192.168.1.201"
int camera_loop()
{

    time_t timer_begin,timer_end;
    raspicam::RaspiCam_Cv camera;
    cv::Mat image;
    int nCount=0;
    //set camera params
    camera.set( cv::CAP_PROP_FORMAT, CV_8UC3 );
    //camera.set( cv::CAP_PROP_FORMAT, CV_8UC3 );

    camera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    //camera.set(cv::CAP_PROP_FRAME_WIDTH,  1280);
    //camera.set(cv::CAP_PROP_FRAME_HEIGHT, 960);


    //camera.set(cv::CAP_PROP_BUFFERSIZE, 3);

    bool res = camera.set(cv::CAP_PROP_FPS, 30);

    //Open camera
    cout<<"Opening camera..."<<endl;
    if (!camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    cout<<"Initalising camera"<<endl;

    // sleep to allow camera to adjust settings
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    cout<<"Starting Stream "<<endl;

    int sock = 0;
	struct sockaddr_in serv_addr;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, PI_SERVER_IP, &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}


    int n;
    rf::FrameHeader header;  // information about the frame being sent
    int img_size;
    while(run_camera)
    {
        // get image
        camera.grab();
        camera.retrieve (image);

        // get meta data
        header.currenct_clock = rf::microSinceEpoc();
        header.frame_timestamp = camera.getFrameCallbackTime();
        header.height = image.rows;
        header.width = image.cols;
        header.type = image.type();
        img_size = image.total()*image.channels();

        // send frame header
        n = write(sock, &header, sizeof(header));
        if (n < 0) {
            cerr << "ERROR writing to socket" << endl;
            break;
        }

        // send frame over socket
        n = write(sock, image.data, img_size);
        if (n < 0) {
            cerr << "ERROR writing to socket" << endl;
            break;
        }
        cout<<"Client: frame header "<< header.currenct_clock << " " << header.frame_timestamp << " "
        << header.height<< " " << header.width << " " << header.type << endl;
    }

    cout<<"Stop camera..."<<endl;
    camera.release();
    cout<<"Close socket"<<endl;
    close(sock);

    return 0;
}



int main ( int argc, char **argv ) {

    boost::thread t(camera_loop); // Separate thread for loop.

    // Wait for input character (this will suspend the main thread, but the loop
    // thread will keep running).
    cin.get();

    // Set the atomic boolean to true. The loop thread will exit from
    // loop and terminate.
    run_camera = false;

    return 1;
}

