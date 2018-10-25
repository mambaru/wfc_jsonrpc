cmake_minimum_required(VERSION 2.8)

project(wfc_jsonrpc)

include_directories(package)
include_directories(utilites)

add_subdirectory(utilites)
add_subdirectory(package)

if (BUILD_TESTING)
  add_subdirectory(tests)
endif()
