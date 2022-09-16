#pragma once

#include <string>
#include <vector>

namespace wfc{ namespace jsonrpc{ 
  
struct extract_config
{
  std::string target;
  bool stream_mode = false;
  bool show_parse_errors = true;
};

}}
