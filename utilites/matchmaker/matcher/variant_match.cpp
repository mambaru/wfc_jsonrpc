
#include "variant_match.hpp"
#include "ibuilder.hpp"

namespace wfc{ namespace jsonrpc{ 

variant_match::variant_match( std::shared_ptr<ibuilder> builder )
  : _builder(builder)
{
}

bool variant_match::configure(const char* beg, const char* end, json::json_error& err)  
{
  typedef std::pair<const char*, const char*> field_pair_t;
  typedef std::vector< field_pair_t > pair_list_t;
  typedef json::vector_of< json::iterator_pair<field_pair_t> > pair_list_json;
  pair_list_t pair_list;
  pair_list_json::serializer()(pair_list, beg, end, &err);
  if (err)
    return false;
  
  _matchers.clear();
  for (auto p : pair_list)
  {
    // Передаем end, а не p.second, чтобы в случае ошибки получить правильное место где она 
    auto m = _builder->build_value_nv(p.first, end, err);
    if ( m==nullptr || err )  return false;
    if (!m->configure(p.first, end, err))
      return false;
    _matchers.push_back(m);
  }
  return true;
}

bool variant_match::match(const char* beg, const char* end, json::json_error& err) 
{
  if ( _matchers.empty() )
    return true;
  
  for (auto m : _matchers)
  {
    if ( m->match(beg, end, err) )
      return true;
  }
  return false;
}


}}
