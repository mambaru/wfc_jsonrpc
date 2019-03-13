set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${UNIT_TEST_BIN_OUTPUT_DIR})                                                                                                                  

add_executable(
  package_test
    matchmaker_suite.cpp
    package_suite.cpp
    package_test.cpp
    ${FAS_TESTING_CPP}
)

target_link_libraries(package_test wfc_jsonrpc)
add_dependencies(package_test matchmaker)
add_test(NAME package_test COMMAND package_test)


