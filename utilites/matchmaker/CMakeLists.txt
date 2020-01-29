add_library(
  matchmaker STATIC
  matchmaker.cpp
  matcher/array_match.cpp
  matcher/builder.cpp
  matcher/full_match.cpp
  matcher/match_factory.cpp
  matcher/object_match.cpp
  matcher/prefix_match.cpp
  matcher/regex_match.cpp
  matcher/true_match.cpp
  matcher/variant_match.cpp
)
wci_targets(matchmaker)

target_link_libraries(matchmaker PRIVATE wjson)
target_include_directories(matchmaker PUBLIC "${wfc_jsonrpc_SOURCE_DIR}/utilites")

