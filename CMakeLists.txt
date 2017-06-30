cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

add_executable (main libsvm-3.22/svm.cpp QRS_detection.cpp ECG.cpp main.cpp)
target_link_libraries (main)
