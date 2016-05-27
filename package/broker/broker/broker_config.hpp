#pragma once

#include <string>
#include <vector>

namespace wfc{ namespace jsonrpc{ 

struct broker_config
{
  struct method_target
  {
    std::string target;
    std::vector<std::string> methods;
  };
  
  std::string target;
  std::vector<method_target> targets;
  std::vector<std::string> reject;
};

} }
