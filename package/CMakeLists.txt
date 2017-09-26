include(BuildInfo)

SET(
  wfc_jsonrpc_source_list

  jsonrpc_package.cpp
  
  broker/broker_module.cpp
  broker/broker/broker_multiton.cpp
  broker/broker/broker.cpp

  queue/queue_module.cpp
  queue/queue/queue_multiton.cpp
  queue/queue/queue.cpp

  repli/repli_module.cpp
  repli/repli/repli_multiton.cpp
  repli/repli/repli.cpp

  hub/hub_module.cpp
  hub/hub/hub_multiton.cpp
  hub/hub/hub.cpp
)

if (WFC_ENABLE_STAT)
SET(
  wfc_jsonrpc_source_list
  ${wfc_jsonrpc_source_list}
  statistics/statistics_module.cpp
  statistics/statistics/statistics_multiton.cpp
  statistics/statistics/statistics.cpp
)
endif()

include_directories(./include/)

add_library(
  wfc_jsonrpc
  ${wfc_jsonrpc_source_list}
)

build_info(wfc_jsonrpc jsonrpc)

install(TARGETS wfc_jsonrpc DESTINATION ${CMAKE_INSTALL_PREFIX})
