include(BuildInfo)

SET(
  wfc_jsonrpc_source_list
  jsonrpc_package.cpp
  module/jsonrpc_module.cpp
  module/gateway_multiton.cpp
  module/broker_multiton.cpp
  module/strand_multiton.cpp
  jsonrpc/repli/repli.cpp
  jsonrpc/repli/repli_multiton.cpp
  jsonrpc/gateway/gateway.cpp
  jsonrpc/broker/broker.cpp
  jsonrpc/strand/strand.cpp
)

add_library(
  wfc_jsonrpc
  ${wfc_jsonrpc_source_list}
)

build_info(wfc_jsonrpc jsonrpc)

install(TARGETS wfc_jsonrpc DESTINATION ${CMAKE_INSTALL_PREFIX})
