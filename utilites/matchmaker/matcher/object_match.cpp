
#include "object_match.hpp"
#include "builder.hpp"

namespace wfc{ namespace jsonrpc{ 

object_match::object_match( std::shared_ptr<ibuilder> builder )
  : _builder(builder)
{
}

bool object_match::configure(const char* beg, const char* end, json::json_error& err) 
{
  typedef std::pair<const char*, const char*> field_pair_t;
  typedef std::pair<field_pair_t, field_pair_t> member_pair_t;
  typedef std::vector< member_pair_t > member_list_t;
  typedef json::iterator_pair<field_pair_t> field_json;
  typedef json::dict< std::vector< json::field< field_json, field_json > >, 10 > object_json;
  
  member_list_t member_list;
  object_json::serializer()(member_list, beg, end, &err);
  if (err)
    return false;
  
  _matchers.clear();
  for (auto p : member_list)
  {
    const char* beg_name  = p.first.first;
    const char* beg_value = p.second.first;
    
    auto name = _builder->build(beg_name, end, err);
    if ( name==nullptr || err) return false;
    
    if ( !name->configure(beg_name, end, err) )
      return false;
    
    auto val = _builder->build(beg_value, end, err);
    if ( val==nullptr || err) return false;
    if ( !val->configure(beg_value, end, err) )
      return false;
    
    _matchers.push_back( std::make_pair(name, val) );
  }
  return true;
}

bool object_match::match(const char* beg, const char* end, json::json_error& err) 
{
  typedef std::pair<const char*, const char*> field_pair_t;
  typedef std::pair<field_pair_t, field_pair_t> member_pair_t;
  typedef std::vector< member_pair_t > member_list_t;
  typedef json::iterator_pair<field_pair_t> field_json;
  typedef json::dict< std::vector< json::field< field_json, field_json > >, 10 > object_json;
  
  if ( _matchers.empty() )
    return true;
  
  if (!json::parser::is_object(beg,end))
    return false;
  
  member_list_t member_list;
  object_json::serializer()(member_list, beg, end, &err);
  if (err)
    return false;
  
  // Нужно совпадение всех  
  for (auto m : _matchers)
  {
    bool matched = false;
    // Проверяем все поля входящего объекта
    for ( auto j : member_list )
    {
      const char* jname = j.first.first;
      const char* jvalue = j.second.first;
      
      if ( !m.first->match(jname, end, err) )
      {
        if (err) return false;
        continue;
      }
      
      if ( !m.second->match(jvalue, end, err) )
      {
        if (err) return false;
        continue;
      }
      matched = true;
      break;
    }
    
    if ( !matched )
    {
      // Нет ни одного совпадения во входящем объекте
      return false;
    }
  }
  // Найдено 
  return true;
}
  

}}
