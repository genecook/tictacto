cmake_minimum_required(VERSION 3.13)

project(tictacto_generator VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-O")

find_package(Boost 1.62.0)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(ttt_games_gen ttt_games_gen.C)

add_executable(ttt_qtable_builder gen_qtables.C qtable.C)

add_executable(ttt_play_from_qtable qtables_play.C qtable.C)


enable_testing()

add_test(NAME gen_games
         COMMAND sh -c "./ttt_games_gen")

add_test(NAME gen_qtable
         COMMAND sh -c "./ttt_qtable_builder")


