# CMakeList.txt : CMake project for UDP_receiver, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("UDP_receiver")

# Add source to this project's executable.
add_executable (UDP_receiver "UDP_receiver.c" "UDP_receiver.h")

find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET UDP_receiver PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(UDP_receiver SQLite::SQLite3)


# TODO: Add tests and install targets if needed.