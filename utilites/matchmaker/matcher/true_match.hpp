#pragma once

#include "imatcher.hpp"

namespace wfc{ namespace jsonrpc{

class true_match
  : public imatcher
{
public:
  virtual bool configure(const char* , const char* , wjson::json_error& ) override;
  virtual bool match(const char*, const char* , wjson::json_error& ) override;
};

}}
