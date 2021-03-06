
#include "array_match.hpp"
#include "ibuilder.hpp"

namespace wfc{ namespace jsonrpc{

array_match::array_match( const std::shared_ptr<ibuilder>& builder )
  : _builder( builder )
{
}

bool array_match::configure(const char* beg, const char* end, wjson::json_error& err)
{
  typedef std::pair<const char*, const char*> field_pair_t;
  typedef std::vector< field_pair_t > pair_list_t;
  typedef wjson::vector_of< wjson::iterator_pair<field_pair_t> > pair_list_json;
  pair_list_t pair_list;
  pair_list_json::serializer()(pair_list, beg, end, &err);
  if (err)
    return false;

  _matchers.clear();
  for (auto p : pair_list)
  {
    // Передаем end, а не p.second, чтобы в случае ошибки получить правильное место где она
    auto m = _builder->build_value(p.first, end, err);
    if ( m==nullptr || err )  return false;
    if (!m->configure(p.first, end, err))
      return false;
    _matchers.push_back(m);
  }
  return true;
}

bool array_match::match(const char* beg, const char* end, wjson::json_error& err)
{
  if ( _matchers.empty() )
    return true;

  if (!wjson::parser::is_array(beg,end))
    return false;

  typedef std::pair<const char*, const char*> field_pair_t;
  typedef std::vector< field_pair_t > pair_list_t;
  typedef wjson::vector_of< wjson::iterator_pair<field_pair_t> > pair_list_json;
  pair_list_t pair_list;
  pair_list_json::serializer()(pair_list, beg, end, &err);
  if (err) return false;

  return std::all_of( std::begin(_matchers), std::end(_matchers), [&](const matcher_list::value_type& m)
  {
    return std::any_of(std::begin(pair_list), std::end(pair_list), [&](const pair_list_t::value_type& p)
    {
      return m->match(p.first, end, err);
    });
  });
}

}}
