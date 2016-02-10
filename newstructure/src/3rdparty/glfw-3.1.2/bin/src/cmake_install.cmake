# Install script for directory: /Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/Debug/libglfw.3.1.dylib"
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/Debug/libglfw.3.dylib"
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/Debug/libglfw.dylib"
      )
    foreach(file
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.3.1.dylib"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.3.dylib"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.dylib"
        )
      if(EXISTS "${file}" AND
         NOT IS_SYMLINK "${file}")
        execute_process(COMMAND "/usr/bin/install_name_tool"
          -id "/usr/local/lib/libglfw.3.dylib"
          "${file}")
        if(CMAKE_INSTALL_DO_STRIP)
          execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "${file}")
        endif()
      endif()
    endforeach()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/Release/libglfw.3.1.dylib"
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/Release/libglfw.3.dylib"
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/Release/libglfw.dylib"
      )
    foreach(file
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.3.1.dylib"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.3.dylib"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.dylib"
        )
      if(EXISTS "${file}" AND
         NOT IS_SYMLINK "${file}")
        execute_process(COMMAND "/usr/bin/install_name_tool"
          -id "/usr/local/lib/libglfw.3.dylib"
          "${file}")
        if(CMAKE_INSTALL_DO_STRIP)
          execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "${file}")
        endif()
      endif()
    endforeach()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/MinSizeRel/libglfw.3.1.dylib"
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/MinSizeRel/libglfw.3.dylib"
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/MinSizeRel/libglfw.dylib"
      )
    foreach(file
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.3.1.dylib"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.3.dylib"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.dylib"
        )
      if(EXISTS "${file}" AND
         NOT IS_SYMLINK "${file}")
        execute_process(COMMAND "/usr/bin/install_name_tool"
          -id "/usr/local/lib/libglfw.3.dylib"
          "${file}")
        if(CMAKE_INSTALL_DO_STRIP)
          execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "${file}")
        endif()
      endif()
    endforeach()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/RelWithDebInfo/libglfw.3.1.dylib"
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/RelWithDebInfo/libglfw.3.dylib"
      "/Users/pourya/Desktop/platform/repos/tetcutter/src/3rdparty/glfw-3.1.2/bin/src/RelWithDebInfo/libglfw.dylib"
      )
    foreach(file
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.3.1.dylib"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.3.dylib"
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglfw.dylib"
        )
      if(EXISTS "${file}" AND
         NOT IS_SYMLINK "${file}")
        execute_process(COMMAND "/usr/bin/install_name_tool"
          -id "/usr/local/lib/libglfw.3.dylib"
          "${file}")
        if(CMAKE_INSTALL_DO_STRIP)
          execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "${file}")
        endif()
      endif()
    endforeach()
  endif()
endif()

