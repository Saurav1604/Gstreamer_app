cmake_minimum_required(VERSION 3.10)
project(GStreamerPlayer C)

set(CMAKE_C_STANDARD 11)

# Find GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

# Include directories
include_directories(${GSTREAMER_INCLUDE_DIRS})

# Add executable
add_executable(gst_player main.c)

# Link libraries
target_link_libraries(gst_player ${GSTREAMER_LIBRARIES})
target_link_directories(gst_player PUBLIC ${GSTREAMER_LIBRARY_DIRS})
target_compile_options(gst_player PUBLIC ${GSTREAMER_CFLAGS_OTHER})
