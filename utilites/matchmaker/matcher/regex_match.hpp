#pragma once

#include "imatcher.hpp"
#include <boost/regex.hpp>
#include <string>

namespace wfc{ namespace jsonrpc{ 

class regex_match
  : public imatcher
{
  boost::regex _reg;
  std::string  _str;
public:
  virtual bool configure(const char* beg, const char* end, json::json_error& err) override;
  
  virtual bool match(const char* beg, const char* end, json::json_error& err) override;
};


}}
