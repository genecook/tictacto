cmake_minimum_required(VERSION 3.13)

project(tictacto_generator VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-O")

set(Boost_DEBUG 1)

#set(Boost_NO_SYSTEM_PATHS ON)
#set(BOOST_DIR "/home/genec/Desktop/tools/boost_1_71_0/tools/boost_install")
#set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}")
#set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")

message("DUDE BOOST ${BOOST_DIR}")
  
find_package(Boost 1.71.0)

#include_directories(${BOOST_INCLUDE_DIRS})

add_executable(ttt_gen main.C)

target_link_libraries(ttt_gen ${Boost_LIBRARIES})



