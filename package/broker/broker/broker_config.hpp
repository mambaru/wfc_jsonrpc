#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <matchmaker/match_mode.hpp>

namespace wfc{ namespace jsonrpc{ 

struct broker_config
{
  struct rule
  {
    std::set<std::string> methods;
    std::string target;
    std::string rule_log;
    int mode = match_mode::Ignore;
    std::shared_ptr<std::string> params;
  };
  
  std::string target;
  std::vector<rule> rules;
  std::vector<std::string> reject;
  
  std::string target_log;
  std::string reject_log;
};

} }
