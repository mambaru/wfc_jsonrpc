cmake_minimum_required(VERSION 2.8)

project(wfc_jsonrpc)

include_directories(package)
include_directories(domain)

add_subdirectory(package)
add_subdirectory(domain)
add_subdirectory(tests)


