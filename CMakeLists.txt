cmake_minimum_required(VERSION 3.16)

project(imageTransmuter)
add_executable(${PROJECT_NAME} 
        src/main.cpp
        src/codecFunctions/BMPCodecFuntions.cpp)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions)

