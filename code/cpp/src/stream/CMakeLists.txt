
##set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED COMPONENTS thread system)
include_directories( ${Boost_INCLUDE_DIRS} )

FIND_PACKAGE(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(raspicam REQUIRED)
include_directories( ${raspicam_INCLUDE_DIRS} )
#link_directories(${raspicam_LIBS})

#list(APPEND CMAKE_CXX_FLAGS "-pthread -std=c++11 ${CMAKE_CXX_FLAGS}")

include_directories(..)
add_executable (camera_stream camera_stream.cpp)
target_link_libraries (camera_stream ${raspicam_LIBS} ${raspicam_CV_LIBS} ${OpenCV_LIBS} ${Boost_LIBS} Boost::thread)

add_executable (camera_reciever camera_reciever.cpp)
target_link_libraries (camera_reciever ${raspicam_LIBS} ${raspicam_CV_LIBS} ${OpenCV_LIBS} ${Boost_LIBS} Boost::thread)

