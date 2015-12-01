#cmake file to compile tetcutter project
cmake_minimum_required(VERSION 2.8)

#project name
project(tetcutter)

##############################################################################
# Custom find modules
##############################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Find/")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} src/3rdparty/loki)

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
# FIND TBB
#########################################################
find_package(TBB REQUIRED)
include_directories(${TBB_INCLUDE_DIRS})
link_directories(${TBB_LIBRARY_DIRS})
add_definitions(${TBB_DEFINITIONS})
if(NOT TBB_FOUND)
    message(ERROR " TBB not found!")
endif(NOT TBB_FOUND)


#########################################################
## other include files
#########################################################
include_directories(src/ src/3rdparty src/3rdparty/loki/include src/3rdparty/bullet3)

#########################################################
## add source files
#########################################################

file(GLOB SRC
	src/3rdparty/lodepng/*.cpp
	src/3rdparty/bullet3/BulletCollision/CollisionDispatch/*.cpp
	src/base/*.cpp
	src/graphics/*.cpp
	src/deformable/*.cpp
	src/main.cpp
	)
	
	
##c++11 required
ADD_DEFINITIONS(
    -std=c++11
)
#########################################################
#Linking
#########################################################
add_executable(tetcutter ${SRC})

target_link_libraries(tetcutter ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY} ${TBB_LIBRARY})
