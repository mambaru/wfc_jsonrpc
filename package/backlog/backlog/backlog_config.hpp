#pragma once

#include <string>
#include <vector>
#include <wfc/core/workflow_options.hpp>

namespace wfc{ namespace jsonrpc{

struct backlog_config
{
  std::string target;
  std::string restore_target;
  std::string path;
  size_t restore_trace = 0;
};

}}
