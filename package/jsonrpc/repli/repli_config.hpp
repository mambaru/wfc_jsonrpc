#pragma once

#include <string>
#include <vector>

namespace wfc{ namespace jsonrpc{ namespace repli{
  
struct repli_config
{
  std::string primary_target;
  std::vector<std::string> targets;
};

}}}
