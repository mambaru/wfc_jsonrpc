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

  batch/batch_module.cpp
  batch/batch/batch_multiton.cpp
  batch/batch/batch.cpp

  hub/hub_module.cpp
  hub/hub/hub_multiton.cpp
  hub/hub/hub.cpp

  backlog/backlog_module.cpp
  backlog/backlog/backlog_multiton.cpp
  backlog/backlog/backlog.cpp
)

if ( WFC_ENABLE_STAT OR STANDALONE )
SET(
  wfc_jsonrpc_source_list
  ${wfc_jsonrpc_source_list}
  statistics/statistics_module.cpp
  statistics/statistics/statistics_multiton.cpp
  statistics/statistics/statistics.cpp
)
endif()

add_library(wfc_jsonrpc ${wfc_jsonrpc_source_list})
wci_targets(wfc_jsonrpc)

target_include_directories(wfc_jsonrpc PUBLIC ${wfc_jsonrpc_SOURCE_DIR})
target_link_libraries(wfc_jsonrpc PUBLIC wfc matchmaker)

build_info(wfc_jsonrpc jsonrpc)

install(TARGETS wfc_jsonrpc DESTINATION ${CMAKE_INSTALL_PREFIX})
