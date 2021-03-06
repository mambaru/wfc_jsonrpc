#pragma once

#include "imatcher.hpp"
#include <string>

namespace wfc{ namespace jsonrpc{

class prefix_match
  : public imatcher
{
  std::string _val;
public:
  virtual bool configure(const char* beg, const char* end, wjson::json_error& err) override;
  virtual bool match(const char* beg, const char* end, wjson::json_error& err) override;
};

}}
