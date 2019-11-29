
#include "regex_match.hpp"

namespace wfc{ namespace jsonrpc{

bool regex_match::configure(const char* beg, const char* end, wjson::json_error& err)
try
{
  wjson::string<>::serializer()(_str, beg, end, &err);
  _reg.assign(_str);
  return !err;
}
catch(...)
{
  return false;
}

bool regex_match::match(const char* beg, const char* end, wjson::json_error& err)
{
  const char* send = wjson::parser::parse_value(beg, end, &err);
  if (err) return false;
  if ( wjson::parser::is_string(beg, end) )
  {
    std::string val;
    wjson::string<>::serializer()(val, beg, end, &err);
    if (err) return false;
    return boost::regex_match(val, _reg );
  }
  else
  {
    // на сыром json
    return boost::regex_match(beg, send, _reg );
  }
}

}}
