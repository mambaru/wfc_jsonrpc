
#include "array_match.hpp"
#include "ibuilder.hpp"

namespace wfc{ namespace jsonrpc{ 

array_match::array_match( std::shared_ptr<ibuilder> builder )
  : _builder(builder)
{
}
  
bool array_match::configure(const char* beg, const char* end, json::json_error& err)
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
    auto m = _builder->build(p.first, end, err);
    if ( m==nullptr || err )  return false;
    if (!m->configure(p.first, end, err))
      return false;
    _matchers.push_back(m);
  }
  return true;
}

bool array_match::match(const char* beg, const char* end, json::json_error& err)
{
  if ( _matchers.empty() )
    return true;
  
  if (!json::parser::is_array(beg,end))
    return false;
  
  typedef std::pair<const char*, const char*> field_pair_t;
  typedef std::vector< field_pair_t > pair_list_t;
  typedef json::vector_of< json::iterator_pair<field_pair_t> > pair_list_json;
  pair_list_t pair_list;
  pair_list_json::serializer()(pair_list, beg, end, &err);
  if (err) return false;
  
  for (auto m : _matchers)
  {
    bool matched = false;
    for (auto p : pair_list)
    {
      if ( m->match(p.first, end, err) )
      {
        matched = true;
        break;
      }
    }
    if (!matched) 
      return false;
  }
  return true;
}

}}
