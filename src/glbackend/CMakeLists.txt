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
# FIND FREETYPE
#########################################################
find_package(FREETYPE REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
link_directories(${FREETYPE_LIBRARIES})
add_definitions(${FREETYPE_DEFINITIONS})
if(NOT FREETYPE_FOUND)
    message(ERROR " FREETYPE not found!")
endif(NOT FREETYPE_FOUND)

#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

#########################################################
## other include files
#########################################################
include_directories(..
                    ../3rdparty/loki/include
                    ../3rdparty/lodepng)

#########################################################
## add source files
#########################################################

file(GLOB GLBACKEND_SRC
	 *.cpp
         ../3rdparty/lodepng/lodepng.h
         ../3rdparty/lodepng/lodepng.cpp
	)

##c++11 required
ADD_DEFINITIONS(
    -std=c++11
    -march=core2 -msse3
)
#########################################################
#Linking
#########################################################
add_library(glbackend ${GLBACKEND_SRC})

target_link_libraries(glbackend base ${OPENGL_LIBRARIES}  ${GLEW_LIBRARY} ${FREETYPE_LIBRARIES})
