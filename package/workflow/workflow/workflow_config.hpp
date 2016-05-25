#pragma once

#include <string>
#include <vector>
#include <wfc/core/workflow_options.hpp>

namespace wfc{ namespace jsonrpc{

struct workflow_config
{
  std::string target;
  
  // Входящая очередь (основная)
  bool incoming_disabled = false;
  ::wfc::workflow_options incoming;
  
  // Очередь для ответов (опционально)
  bool outgoing_disabled = true;
  ::wfc::workflow_options outgoing;
};

}}
