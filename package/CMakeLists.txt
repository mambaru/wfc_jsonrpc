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

  backlog/backlog_module.cpp
  backlog/backlog/backlog_multiton.cpp
  backlog/backlog/backlog.cpp

  batch/batch_module.cpp
  batch/batch/batch_multiton.cpp
  batch/batch/batch.cpp

  extract/extract_module.cpp
  extract/extract/extract_multiton.cpp
  extract/extract/extract.cpp

  stub/stub_module.cpp
  stub/stub/stub_multiton.cpp
  stub/stub/stub.cpp
)

if ( WFC_ENABLE_STAT)
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
enable_stat(wfc_jsonrpc)

target_include_directories(wfc_jsonrpc PUBLIC ${wfc_jsonrpc_SOURCE_DIR})
target_include_directories(wfc_jsonrpc PUBLIC ${wfc_jsonrpc_SOURCE_DIR}/package/backlog)
target_compile_definitions(wfc_jsonrpc PRIVATE -DBOOST_ASIO_NO_DEPRECATED)

target_link_libraries(wfc_jsonrpc PUBLIC matchmaker wfc )

build_info(TARGET wfc_jsonrpc PREFIX jsonrpc)

install(TARGETS wfc_jsonrpc DESTINATION ${CMAKE_INSTALL_PREFIX})
