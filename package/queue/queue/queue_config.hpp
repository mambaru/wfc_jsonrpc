#pragma once

#include <string>
#include <vector>
#include <wfc/core/workflow_options.hpp>

namespace wfc{ namespace jsonrpc{

struct queue_config
{
  std::string target;
  
  // Входящая очередь (основная)
  bool incoming_disabled = false;
  std::string incoming_workflow;
  //::wfc::workflow_options incoming;
  
  // Очередь для ответов (опционально)
  bool outgoing_disabled = true;
  //::wfc::workflow_options outgoing;
  std::string outgoing_workflow;
};

}}
