set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${WFC_TEST_DIR})

SET(
  broker_source_list
  broker_suite.cpp
  broker_test.cpp
  ${FAS_TESTING_CPP}
)

add_executable(
  broker_test
  ${broker_source_list}
)

add_dependencies(broker_test matchmaker)

target_link_libraries(broker_test matchmaker)

add_test(broker_test broker_test)


