#pragma once

#include <matchmaker/match_mode.hpp>
#include <wfc/json.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{ 
  
struct imatcher;

class matchmaker
{
  struct regular;
  enum class regtype;
public:
  bool reconfigure(int mode, const std::string& jsonconfig, json::json_error& err);
  bool match(const char* beg, const char* end, json::json_error& err);
  bool match(const std::string& json, json::json_error& err);
private:
  std::shared_ptr<imatcher> _matcher;
};


}}
