#pragma once

#include "imatcher.hpp"
#include <string>

namespace wfc{ namespace jsonrpc{ 

class full_match
  : public imatcher
{
  std::string _sjson;
public:
  virtual bool configure(const char* beg, const char* end, json::json_error& err) override;
  virtual bool match(const char* beg, const char* end, json::json_error& err) override;
};

}}
