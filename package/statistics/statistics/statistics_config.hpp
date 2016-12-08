#pragma once

#include <string>
#include <vector>

namespace wfc{ namespace jsonrpc{ 

struct statistics_config
{
  std::string target;
  std::string time_suffix = ".time";
  std::string size_suffix = ".size";
  std::string request_prefix = "req:";
  std::string notify_prefix  = "ntf:";
  std::string other_time = "other.time";
  std::string other_size = "other.size";

};

}}