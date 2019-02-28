set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})                                                                                                                  

add_executable(
  package_test
    matchmaker_suite.cpp
    package_suite.cpp
    package_test.cpp
    ${FAS_TESTING_CPP}
)

add_dependencies(package_test matchmaker)
find_package(Boost COMPONENTS system regex REQUIRED)
target_link_libraries(package_test wfc_jsonrpc)

enable_testing()
add_test(NAME package_test COMMAND package_test)


