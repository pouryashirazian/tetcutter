#cmake file to compile tetcutter project
cmake_minimum_required(VERSION 2.8)

#debug build
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

#project name
project(tetcutter)

##adding glfw
add_subdirectory(src/3rdparty/glfw-3.1.2/)

##adding freetype font library
add_subdirectory(src/3rdparty/freetype-2.6.3/)
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
## build all libs
#########################################################
add_subdirectory(src/base/)
add_subdirectory(src/glbackend/)
add_subdirectory(src/scene/)
add_subdirectory(src/elastic/)

#########################################################
## other include files
#########################################################
include_directories(src/
                    src/3rdparty
                    src/3rdparty/loki/include
                    src/3rdparty/glfw-3.1.2/include
                    src/3rdparty/freetype-2.6.3/include)

#########################################################
## add source files
#########################################################
file(GLOB SRC
	   src/main.cpp
	  )


##c++11 required
ADD_DEFINITIONS(
    -std=c++11
    -march=core2 -msse3
)
#########################################################
#Linking
#########################################################
add_executable(tetcutter ${SRC})

#glfw  ${GLUT_LIBRARY}
target_link_libraries(tetcutter glfw base scene elastic glbackend ${FREETYPE_LIBRARIES} ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES}  ${GLEW_LIBRARY} ${TBB_LIBRARY})
