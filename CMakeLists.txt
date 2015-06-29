cmake_minimum_required(VERSION 3.1) # We need CMAKE_C_STANDARD variable
project(Chizu)

# Add custom cmake modules to find SDL
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

add_subdirectory(src)
