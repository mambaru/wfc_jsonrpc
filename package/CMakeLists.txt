include(BuildInfo)

SET(
  wfc_jsonrpc_source_list
  jsonrpc_package.cpp
  
  broker/broker_module.cpp
  broker/broker/broker_multiton.cpp
  broker/broker/broker.cpp

  workflow/workflow_module.cpp
  workflow/workflow/workflow_multiton.cpp
  workflow/workflow/workflow.cpp
  
  module/jsonrpc_module.cpp
  module/gateway_multiton.cpp
  module/strand_multiton.cpp
  jsonrpc/repli/repli.cpp
  jsonrpc/repli/repli_multiton.cpp
  jsonrpc/gateway/gateway.cpp
  
  jsonrpc/strand/strand.cpp
)

include_directories(./include/)

add_library(
  wfc_jsonrpc
  ${wfc_jsonrpc_source_list}
)

build_info(wfc_jsonrpc jsonrpc)

install(TARGETS wfc_jsonrpc DESTINATION ${CMAKE_INSTALL_PREFIX})
