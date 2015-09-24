include(BuildInfo)

SET(
  wfc_jsonrpc_source_list
  jsonrpc_package.cpp
  module/jsonrpc_module.cpp
  module/conveyor_multiton.cpp
  jsonrpc/conveyor/conveyor.cpp
)

add_library(
  wfc_jsonrpc
  ${wfc_jsonrpc_source_list}
)

build_info(wfc_jsonrpc jsonrpc)

install(TARGETS wfc_jsonrpc DESTINATION ${CMAKE_INSTALL_PREFIX})
