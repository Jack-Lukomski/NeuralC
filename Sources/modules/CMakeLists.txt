cmake_minimum_required(VERSION 3.5)

add_library(nn NeuralNetwork.cpp)
add_library(activationfunctions A_Func.cpp)

target_link_libraries(nn activationfunctions)