project (robot)

cmake_minimum_required(VERSION 3.10)

INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(gcc GNU)
CMAKE_FORCE_CXX_COMPILER(g++ GNU)

if(CMAKE_COMPILER_IS_GNUCXX)
	message("GCC Enabled")
    add_definitions(-std=gnu++0x)
    add_definitions(-Wall)
    add_definitions(-m64)
    add_definitions(-fPIC)
endif()

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

#SET (FILTER_SOURCE_FILES
#     common.cpp
#     particlefilter.cpp
#     vasicekparticlefilter.cpp
#     svpfilter.cpp
#)
#add_library (FILTER_LIB ${FILTER_SOURCE_FILES})

# Used for linear algebra routines
FIND_PACKAGE(Armadillo 3.6.0 REQUIRED)

FIND_PACKAGE(Boost)
IF(Boost_FOUND)
  message("Boost Found")
  INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIRS}" "/usr/include/python2.7")
  SET(Boost_USE_STATIC_LIBS OFF)
  SET(Boost_USE_MULTITHREADED ON)
  SET(Boost_USE_STATIC_RUNTIME OFF)
  FIND_PACKAGE(Boost COMPONENTS python)

#  FILE(COPY pfilter_test.py DESTINATION .)

#  ADD_LIBRARY(pfilter SHARED build.cpp)
#  TARGET_LINK_LIBRARIES(pfilter ${Boost_LIBRARIES} ${ARMADILLO_LIBRARY} FILTER_LIB)

  add_executable(hello_world hello_world.cpp)
#  TARGET_LINK_LIBRARIES(test_pfilter ${Boost_LIBRARIES} ${ARMADILLO_LIBRARY} FILTER_LIB)


 
ELSEIF(NOT Boost_FOUND)
  MESSAGE(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
ENDIF()
