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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/work/Projects/appleseed-build/appleseed

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug

# Include any dependencies generated for this target.
include src/appleseed.cli/CMakeFiles/appleseed.cli.dir/depend.make

# Include the progress variables for this target.
include src/appleseed.cli/CMakeFiles/appleseed.cli.dir/progress.make

# Include the compile flags for this target's objects.
include src/appleseed.cli/CMakeFiles/appleseed.cli.dir/flags.make

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.o: src/appleseed.cli/CMakeFiles/appleseed.cli.dir/flags.make
src/appleseed.cli/CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.o: ../src/appleseed.cli/commandlinehandler.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/appleseed.cli/CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.o"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.o -c /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/commandlinehandler.cpp

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.i"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/commandlinehandler.cpp > CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.i

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.s"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/commandlinehandler.cpp -o CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.s

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/main.cpp.o: src/appleseed.cli/CMakeFiles/appleseed.cli.dir/flags.make
src/appleseed.cli/CMakeFiles/appleseed.cli.dir/main.cpp.o: ../src/appleseed.cli/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/appleseed.cli/CMakeFiles/appleseed.cli.dir/main.cpp.o"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/appleseed.cli.dir/main.cpp.o -c /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/main.cpp

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/appleseed.cli.dir/main.cpp.i"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/main.cpp > CMakeFiles/appleseed.cli.dir/main.cpp.i

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/appleseed.cli.dir/main.cpp.s"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/main.cpp -o CMakeFiles/appleseed.cli.dir/main.cpp.s

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.o: src/appleseed.cli/CMakeFiles/appleseed.cli.dir/flags.make
src/appleseed.cli/CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.o: ../src/appleseed.cli/progresstilecallback.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/appleseed.cli/CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.o"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.o -c /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/progresstilecallback.cpp

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.i"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/progresstilecallback.cpp > CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.i

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.s"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/progresstilecallback.cpp -o CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.s

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.o: src/appleseed.cli/CMakeFiles/appleseed.cli.dir/flags.make
src/appleseed.cli/CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.o: ../src/appleseed.cli/stdouttilecallback.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/appleseed.cli/CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.o"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.o -c /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/stdouttilecallback.cpp

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.i"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/stdouttilecallback.cpp > CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.i

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.s"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli/stdouttilecallback.cpp -o CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.s

# Object files for target appleseed.cli
appleseed_cli_OBJECTS = \
"CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.o" \
"CMakeFiles/appleseed.cli.dir/main.cpp.o" \
"CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.o" \
"CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.o"

# External object files for target appleseed.cli
appleseed_cli_EXTERNAL_OBJECTS =

src/appleseed.cli/appleseed.cli: src/appleseed.cli/CMakeFiles/appleseed.cli.dir/commandlinehandler.cpp.o
src/appleseed.cli/appleseed.cli: src/appleseed.cli/CMakeFiles/appleseed.cli.dir/main.cpp.o
src/appleseed.cli/appleseed.cli: src/appleseed.cli/CMakeFiles/appleseed.cli.dir/progresstilecallback.cpp.o
src/appleseed.cli/appleseed.cli: src/appleseed.cli/CMakeFiles/appleseed.cli.dir/stdouttilecallback.cpp.o
src/appleseed.cli/appleseed.cli: src/appleseed.cli/CMakeFiles/appleseed.cli.dir/build.make
src/appleseed.cli/appleseed.cli: src/appleseed.shared/libappleseed.shared.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_atomic-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_chrono-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_date_time-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_filesystem-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_regex-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_system-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_thread-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_wave-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_serialization-mt.a
src/appleseed.cli/appleseed.cli: src/appleseed/libappleseed.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/libpng.dylib
src/appleseed.cli/appleseed.cli: /usr/lib/libz.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/libxerces-c.so
src/appleseed.cli/appleseed.cli: /usr/lib/libz.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/libxerces-c.so
src/appleseed.cli/appleseed.cli: /usr/local/lib/liblz4.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/libOpenImageIO.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/liboslexec.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/liboslcomp.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/liboslquery.dylib
src/appleseed.cli/appleseed.cli: src/thirdparty/bcd/libbcd.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libHalf.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/libIex.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/libImath.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/libIlmImf.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/libIlmThread.dylib
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_atomic-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_chrono-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_date_time-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_filesystem-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_regex-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_system-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_thread-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_wave-mt.a
src/appleseed.cli/appleseed.cli: /usr/local/lib/libboost_serialization-mt.a
src/appleseed.cli/appleseed.cli: src/appleseed.cli/CMakeFiles/appleseed.cli.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable appleseed.cli"
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/appleseed.cli.dir/link.txt --verbose=$(VERBOSE)
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && mkdir -p /Users/work/Projects/appleseed-build/appleseed/sandbox/bin/Debug
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && cp /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli/appleseed.cli /Users/work/Projects/appleseed-build/appleseed/sandbox/bin/Debug

# Rule to build all files generated by this target.
src/appleseed.cli/CMakeFiles/appleseed.cli.dir/build: src/appleseed.cli/appleseed.cli

.PHONY : src/appleseed.cli/CMakeFiles/appleseed.cli.dir/build

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/clean:
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli && $(CMAKE_COMMAND) -P CMakeFiles/appleseed.cli.dir/cmake_clean.cmake
.PHONY : src/appleseed.cli/CMakeFiles/appleseed.cli.dir/clean

src/appleseed.cli/CMakeFiles/appleseed.cli.dir/depend:
	cd /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/work/Projects/appleseed-build/appleseed /Users/work/Projects/appleseed-build/appleseed/src/appleseed.cli /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli /Users/work/Projects/appleseed-build/appleseed/cmake-build-debug/src/appleseed.cli/CMakeFiles/appleseed.cli.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/appleseed.cli/CMakeFiles/appleseed.cli.dir/depend

