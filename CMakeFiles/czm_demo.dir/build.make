# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bdgiffin/czm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bdgiffin/czm

# Include any dependencies generated for this target.
include CMakeFiles/czm_demo.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/czm_demo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/czm_demo.dir/flags.make

CMakeFiles/czm_demo.dir/czm_demo.cpp.o: CMakeFiles/czm_demo.dir/flags.make
CMakeFiles/czm_demo.dir/czm_demo.cpp.o: czm_demo.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/bdgiffin/czm/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/czm_demo.dir/czm_demo.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/czm_demo.dir/czm_demo.cpp.o -c /home/bdgiffin/czm/czm_demo.cpp

CMakeFiles/czm_demo.dir/czm_demo.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/czm_demo.dir/czm_demo.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/bdgiffin/czm/czm_demo.cpp > CMakeFiles/czm_demo.dir/czm_demo.cpp.i

CMakeFiles/czm_demo.dir/czm_demo.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/czm_demo.dir/czm_demo.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/bdgiffin/czm/czm_demo.cpp -o CMakeFiles/czm_demo.dir/czm_demo.cpp.s

CMakeFiles/czm_demo.dir/czm_demo.cpp.o.requires:
.PHONY : CMakeFiles/czm_demo.dir/czm_demo.cpp.o.requires

CMakeFiles/czm_demo.dir/czm_demo.cpp.o.provides: CMakeFiles/czm_demo.dir/czm_demo.cpp.o.requires
	$(MAKE) -f CMakeFiles/czm_demo.dir/build.make CMakeFiles/czm_demo.dir/czm_demo.cpp.o.provides.build
.PHONY : CMakeFiles/czm_demo.dir/czm_demo.cpp.o.provides

CMakeFiles/czm_demo.dir/czm_demo.cpp.o.provides.build: CMakeFiles/czm_demo.dir/czm_demo.cpp.o

# Object files for target czm_demo
czm_demo_OBJECTS = \
"CMakeFiles/czm_demo.dir/czm_demo.cpp.o"

# External object files for target czm_demo
czm_demo_EXTERNAL_OBJECTS =

czm_demo: CMakeFiles/czm_demo.dir/czm_demo.cpp.o
czm_demo: CMakeFiles/czm_demo.dir/build.make
czm_demo: /usr/lib/x86_64-linux-gnu/libGLU.so
czm_demo: /usr/lib/x86_64-linux-gnu/libGL.so
czm_demo: /usr/lib/x86_64-linux-gnu/libSM.so
czm_demo: /usr/lib/x86_64-linux-gnu/libICE.so
czm_demo: /usr/lib/x86_64-linux-gnu/libX11.so
czm_demo: /usr/lib/x86_64-linux-gnu/libXext.so
czm_demo: /usr/lib/x86_64-linux-gnu/libglut.so
czm_demo: /usr/lib/x86_64-linux-gnu/libXmu.so
czm_demo: /usr/lib/x86_64-linux-gnu/libXi.so
czm_demo: /usr/lib/libSOIL.a
czm_demo: CMakeFiles/czm_demo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable czm_demo"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/czm_demo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/czm_demo.dir/build: czm_demo
.PHONY : CMakeFiles/czm_demo.dir/build

CMakeFiles/czm_demo.dir/requires: CMakeFiles/czm_demo.dir/czm_demo.cpp.o.requires
.PHONY : CMakeFiles/czm_demo.dir/requires

CMakeFiles/czm_demo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/czm_demo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/czm_demo.dir/clean

CMakeFiles/czm_demo.dir/depend:
	cd /home/bdgiffin/czm && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bdgiffin/czm /home/bdgiffin/czm /home/bdgiffin/czm /home/bdgiffin/czm /home/bdgiffin/czm/CMakeFiles/czm_demo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/czm_demo.dir/depend
