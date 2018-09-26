#pragma once

#include <wfc/json.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{ 

struct imatcher;

struct ibuilder
{
  virtual ~ibuilder(){}
  virtual std::shared_ptr<imatcher> build(const char* beg, const char* end, json::json_error& err) = 0;
  virtual std::shared_ptr<imatcher> build_nv(const char* beg, const char* end, json::json_error& err) = 0;
};


}}
