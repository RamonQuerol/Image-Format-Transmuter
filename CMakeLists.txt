cmake_minimum_required(VERSION 3.16)

project(imageTransmuter)

# Find the external libraries
find_package(ZLIB REQUIRED)

#For debugging
#set(CMAKE_CXX_FLAGS "-fsanitize=address")

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
        src/codecFunctions/png/pngManagement/pngPixelManagement.cpp
        src/codecFunctions/jpg/jpgCodecFunctions.cpp
        src/codecFunctions/jpg/jpgOther/jpgUtils.cpp
        src/codecFunctions/jpg/jpgClasses/jpgHuffmanTree.cpp
        src/codecFunctions/jpg/jpgAlgorithms/jpgDCT.cpp
        src/codecFunctions/jpg/jpgAlgorithms/jpgZigzag.cpp
        src/codecFunctions/jpg/jpgAlgorithms/jpgDiffEncoding.cpp
        src/codecFunctions/jpg/jpgAlgorithms/jpgQuantization.cpp
        src/codecFunctions/jpg/jpgAlgorithms/jpgDecompression.cpp
        src/codecFunctions/jpg/jpgManagement/jpgPixelManagement.cpp
        src/codecFunctions/jpg/jpgManagement/jpgSegmentManagement.cpp
        src/codecFunctions/jpg/jpgManagement/jpgComponentManagement.cpp)

# Link the project headers
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/utils)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/bmp)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/bmp/bmpManagement)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/png)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/png/pngAlgorithms)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/png/pngManagement)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/jpg)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/jpg/jpgOther)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/jpg/jpgClasses)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/jpg/jpgAlgorithms)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/codecFunctions/jpg/jpgManagement)



# Link the external libraries
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)