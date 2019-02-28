cmake_minimum_required(VERSION 3.1)

project(wfc_jsonrpc)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ci)

wfc_libs()

include_directories(utilites)
add_subdirectory(utilites)
add_subdirectory(package)

if ( BUILD_TESTING )
  enable_testing()
  include_directories(package)
  add_subdirectory(tests)
endif()
