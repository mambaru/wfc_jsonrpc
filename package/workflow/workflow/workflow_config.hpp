#pragma once

#include <string>
#include <vector>
#include <wfc/core/workflow_options.hpp>

namespace wfc{ namespace jsonrpc{

  /*
struct workflow_options: ::wfc::workflow_options
{
  // time_t delay_ms = 0;    // задержка в миллисекундах (todo: вынести в wfc::workflow_config_jsonex)
  // size_t rate_limit = 0;  // ограничение скорости (в сообщениях в секунду)
};
*/
  
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

} }
