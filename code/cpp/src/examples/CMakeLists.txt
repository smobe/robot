
find_package(Boost REQUIRED COMPONENTS thread system)
include_directories( ${Boost_INCLUDE_DIRS} )

FIND_PACKAGE(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(raspicam REQUIRED)
include_directories( ${raspicam_INCLUDE_DIRS} )

add_executable (opencv_camera opencv_camera.cpp)
target_link_libraries (opencv_camera  ${raspicam_LIBS} ${raspicam_CV_LIBS} ${OpenCV_LIBS} ${Boost_LIBS})

add_executable (filter_2d filter_2d.cpp)
target_link_libraries (filter_2d  ${raspicam_LIBS} ${raspicam_CV_LIBS} ${OpenCV_LIBS} ${Boost_LIBS})

add_executable (hsv_conversion hsv_conversion.cpp)
target_link_libraries (hsv_conversion  ${raspicam_LIBS} ${raspicam_CV_LIBS} ${OpenCV_LIBS} ${Boost_LIBS})

add_executable (watershed watershed.cpp)
target_link_libraries (watershed  ${raspicam_LIBS} ${raspicam_CV_LIBS} ${OpenCV_LIBS} ${Boost_LIBS})

add_executable (tracking tracking.cpp)
target_link_libraries (tracking  ${raspicam_LIBS} ${raspicam_CV_LIBS} ${OpenCV_LIBS} ${Boost_LIBS})


