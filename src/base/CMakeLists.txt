#cmake file to compile tetcutter project
cmake_minimum_required(VERSION 2.8)

#debug build
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")


#########################################################
## other include files
#########################################################
include_directories(..
                    ../3rdparty/loki/include
                    ../3rdparty/tbb/include
                    ../3rdparty/lodepng)

#########################################################
## add source files
#########################################################

file(GLOB BASE_SRC
         *.h
	 *.cpp
	)

##c++11 required
ADD_DEFINITIONS(
    -std=c++11
    -march=core2 -msse3
)
#########################################################
#Linking
#########################################################
add_library(base ${BASE_SRC})
