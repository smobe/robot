// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <numeric>
#include <boost/circular_buffer.hpp>

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

/** structure for holding information about a captured frame */
struct FrameInfo {
    FrameInfo(const rf::FrameHeader& header,const uint64_t received_timestamp, const cv::Mat& image) :
    header(header), received_timestamp(received_timestamp), image(image)
    {}
    ~FrameInfo(){}

    rf::FrameHeader header;
    uint64_t received_timestamp;
    cv::Mat image;

};

class FrameBuffer {
public :
    FrameBuffer(int n) : new_frame(false), buffer(n) {}
    ~FrameBuffer(){}

    void add_frame(const rf::FrameHeader& header, const uint64_t received_timestamp, const cv::Mat& image) {
        buffer.push_back(FrameInfo(header, received_timestamp, image));
        new_frame = true;
    }

    std::atomic_bool new_frame;
    boost::circular_buffer<FrameInfo> buffer;
};


/** camera stream runs a camera locally and stored the images in a framebuffer **/
class CameraStream : public FrameBuffer {

public :

    CameraStream(int n) : FrameBuffer(n) {}
    ~CameraStream() {}

    void setFramerate(double fps){
        bool res = camera.set(cv::CAP_PROP_FPS, fps);
    }

    void run()
    {
        cv::Mat image;

        //set camera params
        camera.set( cv::CAP_PROP_FORMAT, CV_8UC3 );
        //camera.set( cv::CAP_PROP_FORMAT, CV_8UC3 );

        camera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        //camera.set(cv::CAP_PROP_FRAME_WIDTH,  1280);
        //camera.set(cv::CAP_PROP_FRAME_HEIGHT, 960);

        //camera.set(cv::CAP_PROP_BUFFERSIZE, 3);

        bool res = camera.set(cv::CAP_PROP_FPS, 25);

        //Open camera
        cout<<"Opening camera..."<<endl;
        if (!camera.open()) {cerr<<"Error opening the camera"<<endl;return;}
        cout<<"Initalising camera"<<endl;

        // sleep to allow camera to adjust settings
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        cout<<"Starting Stream "<<endl;

        rf::FrameHeader header;
        uint64_t received_timestamp;

        while(run_camera)
        {
            // get image
            camera.grab();
            camera.retrieve (image);

            // get frame header imformation
            header.current_clock = rf::microSinceEpoc();
            header.frame_timestamp = camera.getFrameCallbackTime();
            header.height = image.rows;
            header.width = image.cols;
            header.type = image.type();

            //cv::imshow("image3", image);
            //if(cv::waitKey(5) >= 27) break;

            // record the frame buffer
            received_timestamp = header.current_clock;
            buffer.push_back(FrameInfo(header, received_timestamp, image));
            new_frame = true;
        }

        cout<<"Stop camera..."<<endl;
        camera.release();

    }

private :
    raspicam::RaspiCam_Cv camera;
};

class CameraSocket :public FrameBuffer{

public :
    CameraSocket(int n) : FrameBuffer(n) {}
    ~CameraSocket() {}

    void run () {
        int server_fd, new_socket, valread;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

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

        int bytes_read;
        rf::FrameHeader header;
        cv::Mat*  image;
        uint64_t imgSize, buff_ptr, received_timestamp;
        while (run_camera) {
            bytes_read = read( new_socket , &header, sizeof(header));
            if (bytes_read < 0)
            {
                cerr << "error read data frame header from socket " << endl;
                break;
            }
            //cout<<"Server: frame header "<< header.current_clock << " " << header.frame_timestamp << " " << header.height<< " " << header.width << " " << header.type << endl;
            //cout << "buffer read " << bytes_read << " of " << sizeof(header) << endl;

            // record the time the header was recieved
            // this timestamp is used to compute the deviation between the two machines clocks
            received_timestamp = rf::microSinceEpoc();

            // if this is the first frame then create a new mat object
            if (buffer.size() == 0){
                image = new cv::Mat(header.height, header.width, header.type);
            }
            imgSize = image->total()*image->channels();

            // read image from socket
            // the image is likely chunked so read in multiple parts if nessesary
            buff_ptr = 0;
            while (buff_ptr < imgSize){
                bytes_read = read(new_socket, image->data+buff_ptr, imgSize-buff_ptr);
                buff_ptr += bytes_read;
                //cout << "buffer read " << bytes_read << " of " << imgSize << " " << buff_ptr << endl;
                if (bytes_read < 0)
                {
                    cerr << "error read data frame date from socket " << endl;
                    break;
                }
            }

            // record image buffer in frame buffer
            buffer.push_back(FrameInfo(header, received_timestamp, *image));
            new_frame = true;
        }
        delete image;


    }
};

void process_frames(CameraStream& buff1, CameraSocket& buff2,
                    boost::circular_buffer<double>& frame_diffs)
{
    if (buff1.buffer.size() == 0 || buff2.buffer.size() == 0)
        return;

    FrameInfo& frame_info1 = buff1.buffer.back();
    FrameInfo& frame_info2 = buff2.buffer.back();

    int64_t clock_diff = frame_info2.received_timestamp - (frame_info2.header.current_clock+100);
    int64_t delta1 = frame_info1.header.frame_timestamp - frame_info2.header.frame_timestamp;
    int64_t delta2 = delta1 - clock_diff;
    cout << "time difference " << clock_diff << " " << delta1 << " " << delta2 << endl;

    if ( abs(delta2) < 30000){
        cv::imshow("image1", frame_info1.image);
        cv::imshow("image2", frame_info2.image);

        if(delta2 > 0) {
            frame_diffs.push_back(delta2);
            double avg_delta = std::accumulate(frame_diffs.begin(), frame_diffs.end(), 0);
            avg_delta /= frame_diffs.size();
            cout << "avg_delta " << avg_delta << endl;

            if (avg_delta > 1000){
                buff1.setFramerate(25+1);
            }else if (avg_delta < 1000){
                buff1.setFramerate(25-1);
            }
        }

        cv::Mat g1, g2;
        cv::Mat disp, disp8;
        cv::cvtColor(frame_info1.image, g1, cv::COLOR_BGR2GRAY);
        cv::cvtColor(frame_info2.image, g2, cv::COLOR_BGR2GRAY);


        int ndisparities = 96;   /**< Range of disparity */
        int SADWindowSize = 7;
        cv::Ptr<cv::StereoBM> sgbm = cv::StereoBM::create( ndisparities, SADWindowSize );

        /*
        cv::Ptr<cv::StereoSGBM> sbm = cv::StereoSGBM::create(-3,    //int minDisparity
                                    96,     //int numDisparities
                                    7,      //int SADWindowSize
                                    60,     //int P1 = 0
                                    2400,   //int P2 = 0
                                    90,     //int disp12MaxDiff = 0
                                    16,     //int preFilterCap = 0
                                    1,      //int uniquenessRatio = 0
                                    60,     //int speckleWindowSize = 0
                                    20,     //int speckleRange = 0
                                    true);
        */

        sgbm->compute(g1, g2, disp);

        double minVal; double maxVal;
        minMaxLoc( disp, &minVal, &maxVal );
        disp.convertTo( disp, CV_8UC1, 255/(maxVal - minVal));
        //cv::cvtColor(disp, disp8, cv::COLOR_BGR2GRAY);
        //cv::normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);

        cv::imshow("depth", disp);
    }

    cv::Mat diff;
    cv::absdiff(frame_info1.image, frame_info2.image, diff);
    cv::imshow("diff", diff);

}

struct FrameDiffs {

};

int main(int argc, char const *argv[])
{

    CameraStream cam1(6);
    CameraSocket cam2(6);
    boost::circular_buffer<double> frame_diffs(3*30);

    // create camera stream
    boost::function<void()> th_func1 = boost::bind(&CameraStream::run, &cam1);
    boost::thread th1(th_func1);

    boost::function<void()> th_func2 = boost::bind(&CameraSocket::run, &cam2);
    boost::thread th2(th_func2);


    for(;;){
        if (cam1.new_frame ){
            cam1.new_frame = false;
            process_frames(cam1, cam2, frame_diffs);
        }
        if (cam2.new_frame){
            cam2.new_frame = false;
            process_frames(cam1, cam2, frame_diffs);
        }

        if(cv::waitKey(30) >= 27) break;

    }
    //boost::thread t1(camera_socket); // Separate thread for loop.
    //boost::thread t2(camera_loop); // Separate thread for loop.

    cin.get();

}

