#include "full_match.hpp"
#include <cstring>

namespace wfc{ namespace jsonrpc{ 

bool full_match::configure(const char* beg, const char* end, json::json_error& err) 
{
  const char* send = json::parser::parse_string(beg, end, &err);
  if (err) return false;
  _sjson = std::string(beg, send); 
  return true;
}
  
bool full_match::match(const char* beg, const char* end, json::json_error& err)
{
  const char* send = json::parser::parse_value(beg, end, &err);
  if (err) return false;
  if ( static_cast<std::ptrdiff_t>(_sjson.size()) != std::distance(beg, send) )
    return false;
  return std::strncmp(beg, _sjson.c_str(), _sjson.size() ) == 0;
}


}}
