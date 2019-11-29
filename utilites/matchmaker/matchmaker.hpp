#pragma once

#include <matchmaker/match_mode.hpp>
#include <wjson/wjson.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{

struct imatcher;

class matchmaker
{
  struct regular;
  enum class regtype;
public:
  bool reconfigure(int mode, const std::string& jsonconfig, wjson::json_error& err);
  bool match(const char* beg, const char* end, wjson::json_error& err);
  bool match(const std::string& json, wjson::json_error& err);
private:
  std::shared_ptr<imatcher> _matcher;
};


}}
