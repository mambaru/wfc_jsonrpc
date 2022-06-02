
#include "regex_match.hpp"
#include <iostream>

namespace wfc{ namespace jsonrpc{

bool regex_match::configure(const char* beg, const char* end, wjson::json_error& err)
try
{
  wjson::string<>::serializer()(_str, beg, end, &err);
  _reg.assign(_str);
  return !err;
}
catch(const std::exception& e)
{
  std::cerr << "regex_match::configure: " << e.what() << std::endl;
  return this->create_error(beg, end, err);
}
catch(...)
{
  std::cerr << "regex_match::configure: Unknown error" << std::endl;
  return this->create_error(beg, end, err);
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
    if ( !boost::regex_match(val, _reg ) )
      return false;
  }
  else
  {
    // на сыром json
    if ( !boost::regex_match(beg, send, _reg ) )
      return false;
  }
  return true;
}

}}
