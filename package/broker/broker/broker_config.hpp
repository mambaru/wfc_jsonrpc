#pragma once

#include <string>
#include <vector>

namespace wfc{ namespace jsonrpc{ 

struct broker_config
{
  struct rule
  {
    std::string target;
    std::vector<std::string> methods;
  };
  
  std::string target;
  std::vector<rule> rules;
  std::vector<std::string> reject;
};

} }
