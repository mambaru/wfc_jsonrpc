#pragma once

#include <string>
#include <vector>

namespace wfc{ namespace jsonrpc{ 
  
struct repli_config
{
  std::string target;
  std::vector<std::string> reply_targets;
};

}}
