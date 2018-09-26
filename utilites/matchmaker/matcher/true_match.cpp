
#include "true_match.hpp"

namespace wfc{ namespace jsonrpc{ 

bool true_match::configure(const char* , const char* , json::json_error& ) 
{
  return true;
}

bool true_match::match(const char*, const char* , json::json_error& )
{
  return true;
}

}}
