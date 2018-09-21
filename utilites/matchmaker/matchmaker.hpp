#pragma once

#include <matchmaker/match_mode.hpp>
#include <wfc/mutex.hpp>
#include <wfc/json.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{ 
  
class matchmaker
{
  struct regular;
  enum class regtype;
public:
  bool reconfigure(int mode, const std::string& jsonconfig, json::json_error& err);
  bool match(const char* beg, const char* end, json::json_error& err);
  bool match(const std::string& json, json::json_error& err);
private:
  bool reconfigure_(int mode, std::string::const_iterator beg, std::string::const_iterator end, json::json_error& err);
  std::string getstr_(std::string::const_iterator beg, std::string::const_iterator end, json::json_error& err);
  bool match_(const char* beg, const char* end, json::json_error& err);
private:
  typedef rwlock<std::mutex> mutex_type;
  std::shared_ptr<regular> _regular;
  int _mode = match_mode::FullMatch;
  mutable mutex_type _mutex;
  regtype _regtype;
};


}}
