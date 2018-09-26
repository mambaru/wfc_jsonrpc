#pragma once

#include <wfc/json.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{ 

  struct imatcher
{
  virtual ~imatcher() {}
  virtual bool configure(const char* beg, const char* end, json::json_error&) = 0;
  virtual bool match(const char* beg, const char* end, json::json_error& err) = 0;
};


}}
