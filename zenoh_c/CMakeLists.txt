cmake_minimum_required(VERSION 3.20)
project(zenoh_c LANGUAGES C)

find_package(zenohc REQUIRED)

# get all build target
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

foreach(file ${files})
  get_filename_component(target ${file} NAME_WE)
  add_executable(${target} ${file})
  target_link_libraries(${target} PRIVATE zenohc::lib)
endforeach()