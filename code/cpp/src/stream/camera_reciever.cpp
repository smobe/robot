// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <thread>
#include <iostream>

#include <boost/thread.hpp>
#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>
#include <utils.h>

#define PORT 8888
#define BUFF_SIZE 921600

using namespace std;

void camera_loop();
void camera_socket();

std::atomic_bool run_camera(true);

int main(int argc, char const *argv[])
{
    boost::thread t1(camera_socket); // Separate thread for loop.
    boost::thread t2(camera_loop); // Separate thread for loop.

    cin.get();

}

void camera_loop()
{
    raspicam::RaspiCam_Cv camera;
    cv::Mat image;

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
    if (!camera.open()) {cerr<<"Error opening the camera"<<endl;return;}
    cout<<"Initalising camera"<<endl;

    // sleep to allow camera to adjust settings
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    cout<<"Starting Stream "<<endl;

    while(run_camera)
    {
        // get image
        camera.grab();
        camera.retrieve (image);

        cv::imshow("image1", image);
        if(cv::waitKey(30) >= 27) break;
    }

    cout<<"Stop camera..."<<endl;
    camera.release();

}


void camera_socket () {
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[BUFF_SIZE] = {0};
	//char *hello = "Hello from server";

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
												&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address,
								sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
					(socklen_t*)&addrlen))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	int readResult;
	rf::FrameHeader header;

	cv::Mat*  image = NULL;
    uint64_t imgSize;

    //cout << "image size " << imgSize << endl;

    uint64_t buff_ptr;
	while (true) {
        readResult = read( new_socket , &header, sizeof(header));
        if (readResult < 0)
        {
            cerr << "error read data frame header from socket " << endl;
            break;
        }
        //cout<<"Server: frame header "<< header.currenct_clock << " " << header.frame_timestamp << " " << header.height<< " " << header.width << " " << header.type << endl;
        //cout << "buffer read " << readResult << " of " << sizeof(header) << endl;

        // if this is the first frame then create a new mat object
        if (!image){
            image = new cv::Mat(header.height, header.width, header.type);
        }
        imgSize = image->total()*image->channels();

        buff_ptr = 0;
        while (buff_ptr < imgSize){
            readResult = read(new_socket, image->data+buff_ptr, imgSize);
            buff_ptr += readResult;
            //  cout << "buffer read " << readResult << " of " << imgSize << " " << buff_ptr << endl;
            if (readResult < 0)
            {
                cerr << "error read data frame date from socket " << endl;
                break;
            }
        }

        cv::imshow("image2", *image);
        if(cv::waitKey(30) >= 27) break;

    }

	delete image;

}

