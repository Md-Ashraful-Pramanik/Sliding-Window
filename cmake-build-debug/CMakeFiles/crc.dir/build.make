# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
CMAKE_COMMAND = /snap/clion/135/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/135/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ashraful/Desktop/DLL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ashraful/Desktop/DLL/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/crc.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/crc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/crc.dir/flags.make

CMakeFiles/crc.dir/crc_test.c.o: CMakeFiles/crc.dir/flags.make
CMakeFiles/crc.dir/crc_test.c.o: ../crc_test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ashraful/Desktop/DLL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/crc.dir/crc_test.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/crc.dir/crc_test.c.o   -c /home/ashraful/Desktop/DLL/crc_test.c

CMakeFiles/crc.dir/crc_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/crc.dir/crc_test.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ashraful/Desktop/DLL/crc_test.c > CMakeFiles/crc.dir/crc_test.c.i

CMakeFiles/crc.dir/crc_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/crc.dir/crc_test.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ashraful/Desktop/DLL/crc_test.c -o CMakeFiles/crc.dir/crc_test.c.s

# Object files for target crc
crc_OBJECTS = \
"CMakeFiles/crc.dir/crc_test.c.o"

# External object files for target crc
crc_EXTERNAL_OBJECTS =

crc: CMakeFiles/crc.dir/crc_test.c.o
crc: CMakeFiles/crc.dir/build.make
crc: CMakeFiles/crc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ashraful/Desktop/DLL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable crc"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/crc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/crc.dir/build: crc

.PHONY : CMakeFiles/crc.dir/build

CMakeFiles/crc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/crc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/crc.dir/clean

CMakeFiles/crc.dir/depend:
	cd /home/ashraful/Desktop/DLL/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ashraful/Desktop/DLL /home/ashraful/Desktop/DLL /home/ashraful/Desktop/DLL/cmake-build-debug /home/ashraful/Desktop/DLL/cmake-build-debug /home/ashraful/Desktop/DLL/cmake-build-debug/CMakeFiles/crc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/crc.dir/depend

