#pragma once

#include <string>
#include <vector>

namespace wfc{ namespace jsonrpc{

struct repli_config
{
  std::string target;
  bool notifier = false;
  std::vector<std::string> reply_targets;
};

}}
