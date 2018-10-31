#pragma once

#include "imatcher.hpp"
#include <utility>
#include <memory>
#include <list>


namespace wfc{ namespace jsonrpc{ 

struct ibuilder;

class object_match
  : public imatcher
{
public:
  explicit object_match( const std::shared_ptr<ibuilder>& builder );
  virtual bool configure(const char* beg, const char* end, json::json_error& err) override;
  virtual bool match(const char* beg, const char* end, json::json_error& err) override;
private:
  typedef std::shared_ptr<imatcher> matcher_ptr;
  typedef std::pair<matcher_ptr, matcher_ptr> matcher_pair;
  typedef std::list<matcher_pair> matcher_list;
  std::shared_ptr<ibuilder> _builder;
  matcher_list _matchers;
};

}}
