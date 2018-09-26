
#include "prefix_match.hpp"

namespace wfc{ namespace jsonrpc{ 

bool prefix_match::configure(const char* beg, const char* end, json::json_error& err) 
{
  json::string<>::serializer()(_val, beg, end, &err);
  return !err;
}

bool prefix_match::match(const char* beg, const char* end, json::json_error& err)
{
  if (!json::parser::is_string(beg,end))
    return false;
  if ( static_cast<std::ptrdiff_t>(_val.size()) > (std::distance(beg, end) + 2) )
    return false;
  std::string value;
  json::string<>::serializer()(value, beg, end, &err);
  if ( err )
    return false;
  return value.compare( 0, _val.size(), _val ) == 0;
}

}}
