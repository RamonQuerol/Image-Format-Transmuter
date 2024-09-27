cmake_minimum_required(VERSION 3.16)

project(imageTransmuter)
add_executable(${PROJECT_NAME} 
        src/main.cpp
        src/init.cpp
        src/utils/fileDataManagementUtils.cpp
        src/codecFunctions/bmp/BMPCodecFunctions.cpp
        src/codecFunctions/png/PNGCodecFunctions.cpp
        src/codecFunctions/png/PNGChunkManagement.cpp
        src/codecFunctions/png/PNGAlgorithms.cpp)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/utils)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/bmp)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/png)

