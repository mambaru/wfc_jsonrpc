#pragma once

#include <string>
#include <vector>

namespace wfc{ namespace jsonrpc{ 

struct statistics_config
{
  std::string target;
  bool enable_write_size = true;
  bool enable_error_stat = true;
};

struct statistics_stat
{
  std::string time_suffix = ".time";
  std::string read_size_suffix = ".rsize";
  std::string write_size_suffix = ".wsize";
  std::string error_prefix  = ".err:";
  std::string request_prefix = ".req:";
  std::string notify_prefix  = ".ntf:";
  std::string other_time = ".other.time";
  std::string other_read_size = ".other.rsize";
  std::string other_write_size = ".other.wsize";
};

}}
