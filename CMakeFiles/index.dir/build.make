# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /home/huangcc/cmake/bin/cmake

# The command to remove a file.
RM = /home/huangcc/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/huangcc/xurui/rdma613/rdma_demo_skiplist01

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/huangcc/xurui/rdma613/rdma_demo_skiplist01

# Include any dependencies generated for this target.
include CMakeFiles/index.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/index.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/index.dir/flags.make

CMakeFiles/index.dir/src/index/sc_skiplist.cpp.o: CMakeFiles/index.dir/flags.make
CMakeFiles/index.dir/src/index/sc_skiplist.cpp.o: src/index/sc_skiplist.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huangcc/xurui/rdma613/rdma_demo_skiplist01/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/index.dir/src/index/sc_skiplist.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/index.dir/src/index/sc_skiplist.cpp.o -c /home/huangcc/xurui/rdma613/rdma_demo_skiplist01/src/index/sc_skiplist.cpp

CMakeFiles/index.dir/src/index/sc_skiplist.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/index.dir/src/index/sc_skiplist.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huangcc/xurui/rdma613/rdma_demo_skiplist01/src/index/sc_skiplist.cpp > CMakeFiles/index.dir/src/index/sc_skiplist.cpp.i

CMakeFiles/index.dir/src/index/sc_skiplist.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/index.dir/src/index/sc_skiplist.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huangcc/xurui/rdma613/rdma_demo_skiplist01/src/index/sc_skiplist.cpp -o CMakeFiles/index.dir/src/index/sc_skiplist.cpp.s

CMakeFiles/index.dir/src/index/skiplist.cpp.o: CMakeFiles/index.dir/flags.make
CMakeFiles/index.dir/src/index/skiplist.cpp.o: src/index/skiplist.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huangcc/xurui/rdma613/rdma_demo_skiplist01/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/index.dir/src/index/skiplist.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/index.dir/src/index/skiplist.cpp.o -c /home/huangcc/xurui/rdma613/rdma_demo_skiplist01/src/index/skiplist.cpp

CMakeFiles/index.dir/src/index/skiplist.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/index.dir/src/index/skiplist.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huangcc/xurui/rdma613/rdma_demo_skiplist01/src/index/skiplist.cpp > CMakeFiles/index.dir/src/index/skiplist.cpp.i

CMakeFiles/index.dir/src/index/skiplist.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/index.dir/src/index/skiplist.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huangcc/xurui/rdma613/rdma_demo_skiplist01/src/index/skiplist.cpp -o CMakeFiles/index.dir/src/index/skiplist.cpp.s

# Object files for target index
index_OBJECTS = \
"CMakeFiles/index.dir/src/index/sc_skiplist.cpp.o" \
"CMakeFiles/index.dir/src/index/skiplist.cpp.o"

# External object files for target index
index_EXTERNAL_OBJECTS =

libindex.so: CMakeFiles/index.dir/src/index/sc_skiplist.cpp.o
libindex.so: CMakeFiles/index.dir/src/index/skiplist.cpp.o
libindex.so: CMakeFiles/index.dir/build.make
libindex.so: libnet.so
libindex.so: CMakeFiles/index.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/huangcc/xurui/rdma613/rdma_demo_skiplist01/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library libindex.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/index.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/index.dir/build: libindex.so

.PHONY : CMakeFiles/index.dir/build

CMakeFiles/index.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/index.dir/cmake_clean.cmake
.PHONY : CMakeFiles/index.dir/clean

CMakeFiles/index.dir/depend:
	cd /home/huangcc/xurui/rdma613/rdma_demo_skiplist01 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/huangcc/xurui/rdma613/rdma_demo_skiplist01 /home/huangcc/xurui/rdma613/rdma_demo_skiplist01 /home/huangcc/xurui/rdma613/rdma_demo_skiplist01 /home/huangcc/xurui/rdma613/rdma_demo_skiplist01 /home/huangcc/xurui/rdma613/rdma_demo_skiplist01/CMakeFiles/index.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/index.dir/depend

