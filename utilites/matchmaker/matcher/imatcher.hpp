#pragma once

#include <wjson/wjson.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{

struct imatcher
{
  virtual ~imatcher() {}
  virtual bool configure(const char* beg, const char* end, wjson::json_error&) = 0;
  virtual bool match(const char* beg, const char* end, wjson::json_error& err) = 0;
  virtual bool create_error(const char* beg, const char* end, wjson::json_error& err) 
  {
    wjson::create_error<wjson::error_code::InvalidRequest>(&err, beg, end);
    return false;
  }
};


}}
