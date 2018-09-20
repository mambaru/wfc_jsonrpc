
#include "matchmaker.hpp"
#include <regex>
#include <vector>
#include <iostream>

namespace wfc{ namespace jsonrpc{ 

struct matchmaker::regular
{
  std::string prefix_value;
  std::regex regex_value;
  std::vector<std::string> prefix_list;
  std::vector<std::regex> regex_list;
  std::vector<std::pair<std::string, std::shared_ptr<matchmaker> > > prefix_maker_list;
  std::vector<std::pair<std::regex, std::shared_ptr<matchmaker> > > regex_maker_list;
};

enum class matchmaker::regtype
{
  value,
  list, 
  object
};

bool matchmaker::reconfigure(int mode, const std::string& jsonconfig, json::json_error& err)
{
  std::lock_guard<mutex_type> lk(_mutex);
  return this->reconfigure_(mode, jsonconfig.begin(), jsonconfig.end(), err);
}
  
std::string matchmaker::getstr_(std::string::const_iterator beg, std::string::const_iterator end, json::json_error& err)
{
  std::string str;
  json::string<>::serializer()(str, beg, end, &err);
  return str;
}

bool matchmaker::reconfigure_(int mode, std::string::const_iterator beg, std::string::const_iterator end, json::json_error& err)
{
  std::cout << "-0-" << std::endl;
  _mode = mode;
  _regular = std::make_shared<regular>();
  if ( json::parser::is_string(beg, end) )
  {
    _regtype = regtype::value;
    if ( mode&match_mode::RegexMatchValue )
    {
      try
      {
        auto str = this->getstr_(beg, end, err);
        if ( !err )
        {
          std::cout << str << "=" << std::string(beg, end) << std::endl;
          _regular->regex_value = std::regex(this->getstr_(beg, end, err) );
        }
        else
        {
          std::cout << json::strerror::message_trace(err, beg, end) << std::endl;
        }
      }
      catch(std::regex_error& e)
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
    
    if ( /*mode&match_mode::RegexMatchName ||*/ mode&match_mode::PrefixMatchName)
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
        _regular->regex_maker_list.push_back( std::make_pair(std::regex(p.first), pmatcher) );
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
  read_lock<mutex_type> lk(_mutex);
  return this->match_(beg, end, err);
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
  
  bool s_regex_match(const std::regex& reg, const char* beg, const char* end)
  {
    return regex_match(beg, end, reg );
  }
}


bool matchmaker::match_(const char* beg, const char* end, json::json_error& err)
{
  if ( _regtype == regtype::value )
  {
    if ( _mode & match_mode::FullMatchValue )
    {
      return s_full_match(_regular->prefix_value, beg, end);
    }
    else if ( _mode & match_mode::PrefixMatchValue )
    {
      return s_prefix_match(_regular->prefix_value, beg, end, err);
    }
    else if ( _mode & match_mode::RegexMatchValue )
    {
      return s_regex_match(_regular->regex_value, beg, end);
    }
  }
  else if ( _regtype == regtype::list)
  {
    for (const auto& p : _regular->prefix_list)
    {
      if ( _mode & match_mode::FullMatchValue )
      {
        return s_full_match(p, beg, end);
      }
      else if ( _mode & match_mode::PrefixMatchValue )
      {
        return s_prefix_match(p, beg, end, err);
      }
    }
    for (const auto& p : _regular->regex_list)
    {
      if ( s_regex_match(p, beg, end))
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
      auto eval = json::parser::parse_value(bval, end, &err);
      
      for ( const auto& p: _regular->prefix_maker_list )
      {
        if ( _mode & match_mode::FullMatchName )
        {
          if ( s_full_match(p.first, bname, ename) )
          {
            if (p.second==nullptr || p.second->match(bval, eval, err))
              return true;
          }
        }
        else if ( _mode & match_mode::PrefixMatchName )
        {
          if ( s_prefix_match(p.first, bname, ename, err) )
          {
            if (p.second==nullptr || p.second->match(bval, eval, err))
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
          if (p.second==nullptr || p.second->match(bval, eval, err))
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
