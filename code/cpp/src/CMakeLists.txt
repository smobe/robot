INCLUDE_DIRECTORIES(.)


subdirs(socket)
subdirs(stream)

FIND_PACKAGE(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(opencv opencv.cpp)
target_link_libraries(opencv  ${OpenCV_LIBS})

#set(raspicam_DIR "/usr/local/lib/cmake")

find_package(raspicam REQUIRED)
include_directories( ${raspicam_INCLUDE_DIRS} )

add_executable (simpletest_raspicam simpletest_raspicam.cpp)
target_link_libraries (simpletest_raspicam ${raspicam_LIBS})

add_executable (simpletest_raspicam_cv simpletest_raspicam_cv.cpp)
target_link_libraries (simpletest_raspicam_cv ${raspicam_LIBS} ${raspicam_CV_LIBS} ${OpenCV_LIBS})


