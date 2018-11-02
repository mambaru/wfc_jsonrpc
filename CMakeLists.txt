cmake_minimum_required(VERSION 3.1)

project(wfc_jsonrpc)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ci)

if ( STANDALONE )
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/wfc/cmake)
  set(liblist faslib wjson wlog wflow iow wjrpc wrtstat wfc)
  getlibs(liblist)
endif()

include_directories(.)
include_directories(utilites)

add_subdirectory(utilites)
add_subdirectory(package)

if ( BUILD_TESTING AND STANDALONE )
  add_subdirectory(tests)
endif()
