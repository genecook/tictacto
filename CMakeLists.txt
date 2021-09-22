cmake_minimum_required(VERSION 3.13)

project(tictacto_generator VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-O")

find_package(Boost 1.62.0)

add_executable(ttt_gen main.C)

target_link_libraries(ttt_gen ${Boost_LIBRARIES})

enable_testing()

add_test(NAME test1
         COMMAND sh -c "./ttt_gen")


