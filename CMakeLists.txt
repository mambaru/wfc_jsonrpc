cmake_minimum_required(VERSION 3.1)

project(wfc_jsonrpc)

if ( NOT WFC_READY)
  set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  include(mambaru)
  set(CMAKE_MODULE_PATH "${WFC_DIR}/cmake")
endif()

include_directories(.)
include_directories(utilites)

add_subdirectory(utilites)
add_subdirectory(package)

if ( BUILD_TESTING )
  add_subdirectory(tests)
endif()
