add_executable(
  package_test
    matchmaker_suite.cpp
    package_suite.cpp
    package_test.cpp
)

target_link_libraries(package_test PRIVATE wfc_jsonrpc fas::testing)
add_dependencies(package_test matchmaker)
wci_test(TARGET package_test COMMAND package_test)


