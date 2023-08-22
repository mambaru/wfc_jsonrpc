#pragma once

#include <string>
#include <vector>
#include <wfc/core/workflow_options.hpp>

namespace wfc{ namespace jsonrpc{

enum class backlog_mode
{
  server, client
};

struct backlog_config
{
  backlog_mode mode = backlog_mode::server;
  std::string target;
  std::string restore_target;
  std::string path;
  std::string log;
  bool restore_trace = true;
  size_t restore_rate = 1000;
};

}}
