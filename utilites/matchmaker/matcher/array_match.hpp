#pragma once

#include "imatcher.hpp"
#include <list>

namespace wfc{ namespace jsonrpc{ 

struct ibuilder;
  
class array_match
  : public imatcher
{
  typedef std::shared_ptr<imatcher> matcher_ptr;
  typedef std::list<matcher_ptr> matcher_list;
public:
  explicit array_match( const std::shared_ptr<ibuilder>& builder );
  
  virtual bool configure(const char* beg, const char* end, json::json_error& err) override;
  
  virtual bool match(const char* beg, const char* end, json::json_error& err) override;
  
private:
  std::shared_ptr<ibuilder> _builder;
  matcher_list _matchers;
};

}}
