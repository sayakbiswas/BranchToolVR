cmake_minimum_required(VERSION 2.8)

project("dicom2jpeg")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -Wpedantic -pthread")

# Specify include and source files
#---------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

file(GLOB imebra_include_interface "${CMAKE_CURRENT_SOURCE_DIR}/../../library/imebra/include/*.h")
file(GLOB imebra_sources 
	"${CMAKE_CURRENT_SOURCE_DIR}/../../library/imebra/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../library/base/src/*.cpp"
)

file(GLOB dicom2jpeg_sources 
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Add the source files to the project
#------------------------------------
add_executable(dicom2jpeg ${imebra_sources} ${dicom2jpeg_sources})


