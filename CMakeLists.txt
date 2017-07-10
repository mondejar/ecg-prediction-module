cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
FIND_PACKAGE( Boost COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

add_executable (example libsvm-3.22/svm.cpp QRS_detection.cpp resample/resample.cpp ECG.cpp example.cpp)
target_link_libraries (example ${PYTHON_LIBRARIES} ${Boost_LIBRARIES})
