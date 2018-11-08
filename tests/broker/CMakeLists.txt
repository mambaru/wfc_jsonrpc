
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
find_package(Boost COMPONENTS system regex REQUIRED)
target_link_libraries(broker_test matchmaker ${Boost_LIBRARIES} )

enable_testing()
add_test(broker_test broker_test)


