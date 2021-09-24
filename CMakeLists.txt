cmake_minimum_required(VERSION 3.13)

project(tictacto_generator VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-O")

find_package(Boost 1.62.0)

add_executable(ttt_games_gen ttt_games_gen.C)

target_link_libraries(ttt_games_gen ${Boost_LIBRARIES})

add_executable(ttt_nets_builder gen_neural_network.C)

target_link_libraries(ttt_nets_builder ${Boost_LIBRARIES})


enable_testing()

add_test(NAME gen_games
         COMMAND sh -c "./ttt_games_gen")


