# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.4

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/pourya/Desktop/platform/repos/tetcutter/newstructure

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin

# Include any dependencies generated for this target.
include src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/depend.make

# Include the progress variables for this target.
include src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/progress.make

# Include the compile flags for this target's objects.
include src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/flags.make

src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o: src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/flags.make
src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o: ../src/3rdparty/glfw-3.1.2/tests/reopen.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o"
	cd /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin/src/3rdparty/glfw-3.1.2/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/reopen.dir/reopen.c.o   -c /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/src/3rdparty/glfw-3.1.2/tests/reopen.c

src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/reopen.dir/reopen.c.i"
	cd /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin/src/3rdparty/glfw-3.1.2/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/src/3rdparty/glfw-3.1.2/tests/reopen.c > CMakeFiles/reopen.dir/reopen.c.i

src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/reopen.dir/reopen.c.s"
	cd /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin/src/3rdparty/glfw-3.1.2/tests && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/src/3rdparty/glfw-3.1.2/tests/reopen.c -o CMakeFiles/reopen.dir/reopen.c.s

src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o.requires:

.PHONY : src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o.requires

src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o.provides: src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o.requires
	$(MAKE) -f src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/build.make src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o.provides.build
.PHONY : src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o.provides

src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o.provides.build: src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o


# Object files for target reopen
reopen_OBJECTS = \
"CMakeFiles/reopen.dir/reopen.c.o"

# External object files for target reopen
reopen_EXTERNAL_OBJECTS =

src/3rdparty/glfw-3.1.2/tests/reopen: src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o
src/3rdparty/glfw-3.1.2/tests/reopen: src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/build.make
src/3rdparty/glfw-3.1.2/tests/reopen: src/3rdparty/glfw-3.1.2/src/libglfw3.a
src/3rdparty/glfw-3.1.2/tests/reopen: src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable reopen"
	cd /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin/src/3rdparty/glfw-3.1.2/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/reopen.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/build: src/3rdparty/glfw-3.1.2/tests/reopen

.PHONY : src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/build

src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/requires: src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/reopen.c.o.requires

.PHONY : src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/requires

src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/clean:
	cd /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin/src/3rdparty/glfw-3.1.2/tests && $(CMAKE_COMMAND) -P CMakeFiles/reopen.dir/cmake_clean.cmake
.PHONY : src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/clean

src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/depend:
	cd /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/pourya/Desktop/platform/repos/tetcutter/newstructure /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/src/3rdparty/glfw-3.1.2/tests /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin/src/3rdparty/glfw-3.1.2/tests /Users/pourya/Desktop/platform/repos/tetcutter/newstructure/bin/src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/3rdparty/glfw-3.1.2/tests/CMakeFiles/reopen.dir/depend

