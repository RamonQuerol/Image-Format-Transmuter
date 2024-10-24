cmake_minimum_required(VERSION 3.16)

project(imageTransmuter)

# Find the external libraries
find_package(ZLIB REQUIRED)

# 
add_executable(${PROJECT_NAME} 
        src/main.cpp
        src/init.cpp
        src/utils/enumUtils.cpp
        src/utils/fileDataManagementUtils.cpp
        src/codecFunctions/bmp/bmpCodecFunctions.cpp
        src/codecFunctions/bmp/bmpManagement/bmpColorManagement.cpp
        src/codecFunctions/bmp/bmpManagement/bmpPixelManagement.cpp
        src/codecFunctions/png/pngCodecFunctions.cpp
        src/codecFunctions/png/pngAlgorithms/pngCRC.cpp
        src/codecFunctions/png/pngAlgorithms/pngCompression.cpp
        src/codecFunctions/png/pngAlgorithms/pngFiltering.cpp
        src/codecFunctions/png/pngManagement/pngChunkManagement.cpp
        src/codecFunctions/png/pngManagement/pngColorManagement.cpp
        src/codecFunctions/png/pngManagement/pngPixelManagement.cpp)

# Link the project headers
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/utils)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/bmp)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/bmp/bmpManagement)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/png)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/png/pngAlgorithms)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/png/pngManagement)

# Link the external libraries
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)