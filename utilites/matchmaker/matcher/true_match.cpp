
#include "true_match.hpp"

namespace wfc{ namespace jsonrpc{

bool true_match::configure(const char* , const char* , wjson::json_error& )
{
  return true;
}

bool true_match::match(const char*, const char* , wjson::json_error& )
{
  return true;
}

}}
