PROJECT(gvlad)

cmake_minimum_required(VERSION 2.8)
IF(MSVC)
	SET(CMAKE_CXX_FLAGS "/EHsc")
	ADD_DEFINITIONS( -DBOOST_ALL_NO_LIB )
ENDIF(MSVC)
find_package(Boost COMPONENTS chrono filesystem system REQUIRED)
find_package(OpenCV REQUIRED)

INCLUDE_DIRECTORIES(
${Boost_INCLUDE_DIRS}
${OPENCV_INCLUDE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}
)
SET( LIBRARY_OUTPUT_PATH ${gvlad_BINARY_DIR}/../lib CACHE PATH
     "Output directory for the gvlad library" )
SET( EXECUTABLE_OUTPUT_PATH 
     ${gvlad_BINARY_DIR}/../bin CACHE PATH
     "Output directory for the gvlad executables" )

SUBDIRS(gvlad/lib)
SUBDIRS(gvlad/exe)
SUBDIRS(gvlad/server)
