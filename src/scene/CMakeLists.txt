#cmake file to compile tetcutter project
cmake_minimum_required(VERSION 2.8)

#debug build
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

#########################################################
# FIND GLEW
#########################################################
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
add_definitions(${GLEW_DEFINITIONS})
if(NOT GLEW_FOUND)
    message(ERROR " GLEW not found!")
endif(NOT GLEW_FOUND)

#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)

#########################################################
## other include files
#########################################################
include_directories(..
                    ../3rdparty/loki/include)

#########################################################
## add source files
#########################################################

file(GLOB SCENE_SRC
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
add_library(scene ${SCENE_SRC})

target_link_libraries(scene base glbackend ${GLEW_LIBRARY})
