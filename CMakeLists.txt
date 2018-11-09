cmake_minimum_required(VERSION 3.1)

project(wfc_jsonrpc)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ci)

if ( STANDALONE )
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/wfc/cmake)
  getlibs(faslib wjson wlog wflow iow wjrpc wrtstat wfc)
endif()

include_directories(utilites)

add_subdirectory(utilites)
add_subdirectory(package)

if ( BUILD_TESTING AND STANDALONE )

  include(FindThreads)
  find_package(Boost COMPONENTS system program_options filesystem date_time regex REQUIRED)

  set(
    wfc_liblist
    wfc_jsonrpc
    wfc
    wrtstat
    wlog
    wflow
    wjrpc
    ${Boost_LIBRARIES} 
    ${CMAKE_THREAD_LIBS_INIT} 
  )

  enable_testing()
  include_directories(package)
  add_subdirectory(tests)
endif()
