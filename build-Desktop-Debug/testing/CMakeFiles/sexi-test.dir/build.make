# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/keithh/Development/RamblingMadMan/sexi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug

# Include any dependencies generated for this target.
include testing/CMakeFiles/sexi-test.dir/depend.make

# Include the progress variables for this target.
include testing/CMakeFiles/sexi-test.dir/progress.make

# Include the compile flags for this target's objects.
include testing/CMakeFiles/sexi-test.dir/flags.make

testing/CMakeFiles/sexi-test.dir/main.cpp.o: testing/CMakeFiles/sexi-test.dir/flags.make
testing/CMakeFiles/sexi-test.dir/main.cpp.o: ../testing/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object testing/CMakeFiles/sexi-test.dir/main.cpp.o"
	cd /home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug/testing && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sexi-test.dir/main.cpp.o -c /home/keithh/Development/RamblingMadMan/sexi/testing/main.cpp

testing/CMakeFiles/sexi-test.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sexi-test.dir/main.cpp.i"
	cd /home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug/testing && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/keithh/Development/RamblingMadMan/sexi/testing/main.cpp > CMakeFiles/sexi-test.dir/main.cpp.i

testing/CMakeFiles/sexi-test.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sexi-test.dir/main.cpp.s"
	cd /home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug/testing && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/keithh/Development/RamblingMadMan/sexi/testing/main.cpp -o CMakeFiles/sexi-test.dir/main.cpp.s

# Object files for target sexi-test
sexi__test_OBJECTS = \
"CMakeFiles/sexi-test.dir/main.cpp.o"

# External object files for target sexi-test
sexi__test_EXTERNAL_OBJECTS =

testing/sexi-test: testing/CMakeFiles/sexi-test.dir/main.cpp.o
testing/sexi-test: testing/CMakeFiles/sexi-test.dir/build.make
testing/sexi-test: lib/libsexi.so
testing/sexi-test: testing/CMakeFiles/sexi-test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable sexi-test"
	cd /home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug/testing && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sexi-test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
testing/CMakeFiles/sexi-test.dir/build: testing/sexi-test

.PHONY : testing/CMakeFiles/sexi-test.dir/build

testing/CMakeFiles/sexi-test.dir/clean:
	cd /home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug/testing && $(CMAKE_COMMAND) -P CMakeFiles/sexi-test.dir/cmake_clean.cmake
.PHONY : testing/CMakeFiles/sexi-test.dir/clean

testing/CMakeFiles/sexi-test.dir/depend:
	cd /home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/keithh/Development/RamblingMadMan/sexi /home/keithh/Development/RamblingMadMan/sexi/testing /home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug /home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug/testing /home/keithh/Development/RamblingMadMan/sexi/build-Desktop-Debug/testing/CMakeFiles/sexi-test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : testing/CMakeFiles/sexi-test.dir/depend
