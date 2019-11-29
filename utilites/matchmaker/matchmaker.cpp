
#include "matchmaker.hpp"
#include <boost/regex.hpp>
#include <vector>
#include <iostream>
#include "matcher/builder.hpp"
#include "matcher/imatcher.hpp"

namespace wfc{ namespace jsonrpc{

bool matchmaker::reconfigure(int mode, const std::string& jsonconfig, wjson::json_error& err)
{
  auto bldr = std::make_shared<builder>(mode);
  const char* beg = jsonconfig.c_str();
  const char* end = beg + jsonconfig.size();
  _matcher = bldr->build_value(beg, end, err);
  if (err || _matcher == nullptr)
    return false;
  return _matcher->configure(beg, end, err);
}

bool matchmaker::match(const std::string& json, wjson::json_error& err)
{
  const char* beg = json.c_str();
  return this->match( beg, beg + json.size(), err);
}

bool matchmaker::match(const char* beg, const char* end, wjson::json_error& err)
{
  return _matcher->match(beg, end, err);
}

}}
