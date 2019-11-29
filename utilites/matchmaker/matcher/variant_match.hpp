#pragma once

#include "imatcher.hpp"
#include <list>

namespace wfc{ namespace jsonrpc{

struct ibuilder;

class variant_match
  : public imatcher
{
public:
  explicit variant_match( const std::shared_ptr<ibuilder>& builder );
  virtual bool configure(const char* beg, const char* end, wjson::json_error& err) override;
  virtual bool match(const char* beg, const char* end, wjson::json_error& err) override;
private:
  typedef std::shared_ptr<imatcher> matcher_ptr;
  typedef std::list<matcher_ptr> matcher_list;
  std::shared_ptr<ibuilder> _builder;
  matcher_list _matchers;
};

}}
