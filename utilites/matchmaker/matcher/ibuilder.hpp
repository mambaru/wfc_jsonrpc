#pragma once

#include <wjson/wjson.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{

struct imatcher;

struct ibuilder
{
  virtual ~ibuilder(){}
  virtual std::shared_ptr<imatcher> build_name() = 0;
  virtual std::shared_ptr<imatcher> build_value(const char* beg, const char* end, wjson::json_error& err) = 0;
  virtual std::shared_ptr<imatcher> build_value_nv(const char* beg, const char* end, wjson::json_error& err) = 0;
};


}}
