
#include "matchmaker.hpp"
#include <boost/regex.hpp>
#include <vector>
#include <iostream>

namespace wfc{ namespace jsonrpc{ 

  struct imatcher
{
  virtual ~imatcher() {}
  virtual bool configure(const char* beg, const char* end, json::json_error&) = 0;
  virtual bool match(const char* beg, const char* end, json::json_error& err) = 0;
};

class true_match
  : public imatcher
{
public:
  virtual bool configure(const char* , const char* , json::json_error& ) override 
  {
    return true;
  }
  
  virtual bool match(const char*, const char* , json::json_error& ) override
  {
    return true;
  }
};


class full_match
  : public imatcher
{
  std::string _sjson;
public:
  virtual bool configure(const char* beg, const char* end, json::json_error& err) override 
  {
    const char* send = json::parser::parse_string(beg, end, &err);
    if (err) return false;
    _sjson = std::string(beg, send); 
    return true;
  }
  
  virtual bool match(const char* beg, const char* end, json::json_error& err) override
  {
    const char* send = json::parser::parse_value(beg, end, &err);
    if (err) return false;
    if ( static_cast<std::ptrdiff_t>(_sjson.size()) != std::distance(beg, send) )
      return false;
    return std::strncmp(beg, _sjson.c_str(), _sjson.size() ) == 0;
  }
};

class prefix_match
  : public imatcher
{
  std::string _val;
public:
  virtual bool configure(const char* beg, const char* end, json::json_error& err) override 
  {
    json::string<>::serializer()(_val, beg, end, &err);
    return !err;
  }
  
  virtual bool match(const char* beg, const char* end, json::json_error& err) override
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
};

class regex_match
  : public imatcher
{
  boost::regex _reg;
  std::string  _str;
public:
  virtual bool configure(const char* beg, const char* end, json::json_error& err) override try
  {
    json::string<>::serializer()(_str, beg, end, &err);
    _reg.assign(_str); 
    return !err; 
  } 
  catch(...) 
  { 
    return false;
  }
  
  virtual bool match(const char* beg, const char* end, json::json_error& err) override
  {
    const char* send = json::parser::parse_value(beg, end, &err);
    if (err) return false;
    if ( json::parser::is_string(beg, end) )
    {
      std::string val; 
      json::string<>::serializer()(val, beg, end, &err);
      if (err) return false;
      return boost::regex_match(val, _reg );
    }
    else
    {
      // на сыром json
      return boost::regex_match(beg, send, _reg );
    }
  }
};

class match_factory
{
public:
  match_factory(int mode): _mode(mode){}
  std::shared_ptr<imatcher> create()
  {
    if ( _mode & match_mode::FullMatch )
      return std::make_shared<full_match>();
    if ( _mode & match_mode::PrefixMatch )
      return std::make_shared<prefix_match>();
    if ( _mode & match_mode::RegexMatch )
      return std::make_shared<regex_match>();
    return nullptr;
  }
private:
  int _mode;
};

struct ibuilder
{
  virtual ~ibuilder(){}
  virtual std::shared_ptr<imatcher> build(const char* beg, const char* end, json::json_error& err) = 0;
  virtual std::shared_ptr<imatcher> build_nv(const char* beg, const char* end, json::json_error& err) = 0;
};

class array_match
  : public imatcher
{
  typedef std::shared_ptr<imatcher> matcher_ptr;
  typedef std::list<matcher_ptr> matcher_list;
public:
  array_match( std::shared_ptr<ibuilder> builder )
    : _builder(builder)
  {
  }
  
  virtual bool configure(const char* beg, const char* end, json::json_error& err) override 
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
  
  virtual bool match(const char* beg, const char* end, json::json_error& err) override
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
  
private:
  std::shared_ptr<ibuilder> _builder;
  matcher_list _matchers;
};

class variant_match
  : public imatcher
{
  typedef std::shared_ptr<imatcher> matcher_ptr;
  typedef std::list<matcher_ptr> matcher_list;
public:
  variant_match( std::shared_ptr<ibuilder> builder )
    : _builder(builder)
  {
  }
  
  virtual bool configure(const char* beg, const char* end, json::json_error& err) override 
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
      auto m = _builder->build_nv(p.first, end, err);
      if ( m==nullptr || err )  return false;
      if (!m->configure(p.first, end, err))
        return false;
      _matchers.push_back(m);
    }
    return true;
  }
  
  virtual bool match(const char* beg, const char* end, json::json_error& err) override
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
  
private:
  std::shared_ptr<ibuilder> _builder;
  matcher_list _matchers;
};

class object_match
  : public imatcher
{
  typedef std::shared_ptr<imatcher> matcher_ptr;
  typedef std::pair<matcher_ptr, matcher_ptr> matcher_pair;
  typedef std::list<matcher_pair> matcher_list;
public:
  object_match( std::shared_ptr<ibuilder> builder )
    : _builder(builder)
  {
  }
  
  virtual bool configure(const char* beg, const char* end, json::json_error& err) override 
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
  
  virtual bool match(const char* beg, const char* end, json::json_error& err) override
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
private:
  std::shared_ptr<ibuilder> _builder;
  matcher_list _matchers;
};


class builder
  : public ibuilder
  , public std::enable_shared_from_this<builder>
{
public:
  builder(int mode): _factory(mode){}
  virtual std::shared_ptr<imatcher> build(const char* beg, const char* end, json::json_error& err) override
  {
    return this->build_(beg, end, err, false);
  }
  
  virtual std::shared_ptr<imatcher> build_nv(const char* beg, const char* end, json::json_error& err) override
  {
    return this->build_(beg, end, err, true);
  }
private:
  std::shared_ptr<imatcher> build_(const char* beg, const char* end, json::json_error& err, bool nv) 
  {
    beg = json::parser::parse_space(beg, end, &err);
    if (err || beg==end) return nullptr;
    std::shared_ptr<imatcher> ptr;
    if ( json::parser::is_null(beg, end) )
    {
      ptr = std::make_shared<true_match>();
    }
    else if ( json::parser::is_array(beg, end) )
    {
      if ( nv )
      {
        ptr = std::make_shared<array_match>( this->shared_from_this() );
      }
      else
      {
        ptr = std::make_shared<variant_match>( this->shared_from_this() );
      }
    }
    else if ( json::parser::is_object(beg, end) )
    {
      ptr = std::make_shared<object_match>( this->shared_from_this() );
    }
    else
    {
      ptr = _factory.create();
    }
    return ptr;
  }
  
private:
  match_factory _factory;
};

struct matchmaker::regular
{
  std::string prefix_value;
  boost::regex regex_value;
  std::vector<std::string> prefix_list;
  std::vector<boost::regex> regex_list;
  std::vector<std::pair<std::string, std::shared_ptr<matchmaker> > > prefix_maker_list;
  std::vector<std::pair<boost::regex, std::shared_ptr<matchmaker> > > regex_maker_list;
};

enum class matchmaker::regtype
{
  null,
  value,
  list, 
  object
};

bool matchmaker::reconfigure(int mode, const std::string& jsonconfig, json::json_error& err)
{
  auto bldr = std::make_shared<builder>(mode);
  const char* beg = jsonconfig.c_str();
  const char* end = beg + jsonconfig.size();
  _matcher = bldr->build(beg, end, err);
  if (err || _matcher == nullptr)
    return false;
  return _matcher->configure(beg, end, err);
  /*
  return this->reconfigure_(mode, jsonconfig.begin(), jsonconfig.end(), err);
  */
}
  
std::string matchmaker::getstr_(std::string::const_iterator beg, std::string::const_iterator end, json::json_error& err)
{
  std::string str;
  json::string<>::serializer()(str, beg, end, &err);
  return str;
}

bool matchmaker::reconfigure_(int mode, std::string::const_iterator beg, std::string::const_iterator end, json::json_error& err)
{
  _mode = mode;
  if ( _mode == match_mode::Ignore )
    return true;
  _regular = std::make_shared<regular>();
  if ( json::parser::is_null(beg, end) )
  {
    _regtype = regtype::value;
  }
  else if ( json::parser::is_string(beg, end) )
  {
    _regtype = regtype::value;
    if ( mode&match_mode::RegexMatchValue )
    {
      try
      {
        auto str = this->getstr_(beg, end, err);
        if ( !err )
        {
          _regular->regex_value = boost::regex(str);
        }
        else
        {
          // TODO:
        }
      }
      catch(boost::regex_error& e)
      {
        std::cout << e.what() << std::endl;
        return false;
      }
    }
    else
    {
      if ( mode&match_mode::PrefixMatchValue )
        _regular->prefix_value = this->getstr_(beg, end, err);
      else
        _regular->prefix_value = std::string(beg, end);
    }
  }
  else if ( json::parser::is_array(beg, end) )
  {
    _regtype = regtype::list;
    std::vector<std::string> str_list;
    if ( mode&match_mode::RegexMatchValue )
    {
      json::vector_of_strings<>::serializer()(str_list, beg, end, &err);
      _regular->regex_list.assign(str_list.begin(), str_list.end());
    }
    else
    {
      if ( mode&match_mode::PrefixMatchValue )
      {
        json::vector_of_strings<>::serializer()(str_list, beg, end, &err);
        _regular->prefix_list.assign(str_list.begin(), str_list.end());
      }
      else if ( mode&match_mode::FullMatchValue )
      {
        json::vector_of< json::raw_value<> >::serializer()(str_list, beg, end, &err);
        _regular->prefix_list.assign(str_list.begin(), str_list.end());
      }
    }
  }
  else if ( json::parser::is_object(beg, end) )
  {
    _regtype = regtype::object;
    std::vector< std::pair<std::string, std::string> > dict;
    
    if ( mode&match_mode::PrefixMatchName)
    {
      json::dict_vector< json::raw_value<> >::serializer()(dict, beg, end, &err);
    }
    else 
    {
      // full match name (без сериализации)
      using dict_type = json::dict< std::vector< json::field< json::raw_value<>, json::raw_value<> > > >;
      dict_type::serializer()(dict, beg, end, &err);
    }
    for (const auto& p : dict)
    {
      std::shared_ptr<matchmaker> pmatcher;
      if ( !json::parser::is_null(p.second.begin(), p.second.end()) )
      {
        pmatcher=std::make_shared<matchmaker>();
        if ( !pmatcher->reconfigure_(mode, p.second.begin(), p.second.end(), err) )
          return false;
      }
      
      if ( mode&match_mode::RegexMatchName )
      {
        _regular->regex_maker_list.push_back( std::make_pair(boost::regex(p.first), pmatcher) );
      }
      else
      {
        _regular->prefix_maker_list.push_back( std::make_pair(p.first, pmatcher) );
      }
    }
  }
  else
  {
    json::create_error<json::error_code::InvalidMember>(&err, std::distance(beg, end));
  }
  return !err;
}

bool matchmaker::match(const std::string& json, json::json_error& err)
{
  const char* beg = json.c_str();
  return this->match( beg, beg + json.size(), err);
}

bool matchmaker::match(const char* beg, const char* end, json::json_error& err)
{
  return _matcher->match(beg, end, err);
  //return this->match_(beg, end, err);
}




namespace
{
  bool s_full_match(const std::string& str, const char* beg, const char* end)
  {
    if ( static_cast<std::ptrdiff_t>(str.size()) != std::distance(beg, end) )
      return false;
    return std::strncmp(beg, str.c_str(), str.size() ) == 0;
  }
  
  bool s_prefix_match(const std::string& str, const char* beg, const char* end, json::json_error& err)
  {
    if ( static_cast<std::ptrdiff_t>(str.size()) > (std::distance(beg, end) + 2) )
      return false;
    std::string value;
    json::string<>::serializer()(value, beg, end, &err);
    if ( err )
      return false;
    return value.compare( 0, str.size(), str ) == 0;
  }
  
  bool s_regex_match(const boost::regex& reg, const char* beg, const char* end)
  {
    return boost::regex_match(beg, end, reg );
  }
}


bool matchmaker::match_(const char* beg, const char* end, json::json_error& err)
{
  if ( _mode == match_mode::Ignore )
    return true;
  // end указывает на конец всего сообщения, чтобы в случае ошибки могли указать место 
  const char* vend = json::parser::parse_value(beg, end, &err);
  if (err) return false;

  if ( _regtype == regtype::null )
  {
    return true;
  }
  else if ( _regtype == regtype::value )
  {
    if ( _mode & match_mode::FullMatchValue )
    {
      return s_full_match(_regular->prefix_value, beg, vend);
    }
    else if ( _mode & match_mode::PrefixMatchValue )
    {
      return s_prefix_match(_regular->prefix_value, beg, vend, err);
    }
    else if ( _mode & match_mode::RegexMatchValue )
    {
      return s_regex_match(_regular->regex_value, beg, vend);
    }
  }
  else if ( _regtype == regtype::list)
  {
    for (const auto& p : _regular->prefix_list)
    {
      if ( _mode & match_mode::FullMatchValue )
      {
        return s_full_match(p, beg, vend);
      }
      else if ( _mode & match_mode::PrefixMatchValue )
      {
        return s_prefix_match(p, beg, vend, err);
      }
    }
    for (const auto& p : _regular->regex_list)
    {
      if ( s_regex_match(p, beg, vend))
        return true;
    }
  }
  else if ( _regtype == regtype::object)
  {
    beg = json::parser::parse_space(beg, end, &err);
    if (!json::parser::is_object(beg, end))
      return false;
    ++beg;
    
    while (beg!=end)
    {
      beg = json::parser::parse_space(beg, end, &err);
      if ( !json::parser::is_string(beg, end) ) 
        break;
      auto bname = beg;
      auto ename = json::parser::parse_string(bname, end, &err);
      if (err) return false;
      auto bval = ename;
      bval = json::parser::parse_space(bval, end, &err);
      if ( bval == end || *(bval++)!=':') return false;
      bval = json::parser::parse_space(bval, end, &err);
      if (err) return false;
      
      for ( const auto& p: _regular->prefix_maker_list )
      {
        if ( _mode & match_mode::FullMatchName )
        {
          if ( s_full_match(p.first, bname, ename) )
          {
            if (p.second==nullptr || p.second->match(bval, end, err))
              return true;
          }
        }
        else if ( _mode & match_mode::PrefixMatchName )
        {
          if ( s_prefix_match(p.first, bname, ename, err) )
          {
            if (p.second==nullptr || p.second->match(bval, end, err))
            {
              return true;
            }
          }
        }
      }
      
      for ( const auto& p: _regular->regex_maker_list )
      {
        if ( s_regex_match(p.first, bname, ename) )
        {
          if (p.second==nullptr || p.second->match(bval, end, err))
            return true;
        }
      }
      
      beg = json::parser::parse_member(beg, end, &err);
      beg = json::parser::parse_space(beg, end, &err);
      if (err || beg==end)
        return false;
      if (*(beg++) == '}' )
        return false;
    }
  }
  return false;
}

}}
