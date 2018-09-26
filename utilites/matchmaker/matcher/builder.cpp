
#include "builder.hpp"
#include "true_match.hpp"
#include "array_match.hpp"
#include "variant_match.hpp"
#include "object_match.hpp"

namespace wfc{ namespace jsonrpc{ 

builder::builder(int mode)
  : _factory(mode)
{}

std::shared_ptr<imatcher> builder::build_name()
{
  return _factory.create_name();
}

std::shared_ptr<imatcher> builder::build_value(const char* beg, const char* end, json::json_error& err)
{
  return this->build_value_(beg, end, err, false);
}
  
std::shared_ptr<imatcher> builder::build_value_nv(const char* beg, const char* end, json::json_error& err)
{
  return this->build_value_(beg, end, err, true);
}

std::shared_ptr<imatcher> builder::build_value_(const char* beg, const char* end, json::json_error& err, bool nv) 
{
  beg = json::parser::parse_space(beg, end, &err);
  if (err || beg==end) return nullptr;
  
  std::shared_ptr<imatcher> ptr;
  if ( json::parser::is_null(beg, end) )
  {
    ptr = std::make_shared<true_match>();
  }
  else if ( json::parser::is_array(beg, end) )
  {
    if ( nv )
    {
      ptr = std::make_shared<array_match>( this->shared_from_this() );
    }
    else
    {
      ptr = std::make_shared<variant_match>( this->shared_from_this() );
    }
  }
  else if ( json::parser::is_object(beg, end) )
  {
    ptr = std::make_shared<object_match>( this->shared_from_this() );
  }
  else
  {
    ptr = _factory.create_value();
  }
  return ptr;
}

}}
