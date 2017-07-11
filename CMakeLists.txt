# License
# Boost
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#      http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

#FIND_PACKAGE( Boost COMPONENTS program_options REQUIRED )
#INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

add_executable (example src/libsvm-3.22/svm.cpp src/QRS_detection.cpp src/resample/resample.cpp src/ECG.cpp src/example.cpp)
target_link_libraries (example ${PYTHON_LIBRARIES}) 

# ${Boost_LIBRARIES})
