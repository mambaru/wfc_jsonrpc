
#include "match_factory.hpp"
#include "../match_mode.hpp"

#include "full_match.hpp"
#include "prefix_match.hpp"
#include "regex_match.hpp"

namespace wfc{ namespace jsonrpc{ 

match_factory::match_factory(int mode)
  : _mode(mode)
{}

std::shared_ptr<imatcher> match_factory::create_name()
{
  if ( _mode & match_mode::FullMatchName )
  {
    return std::make_shared<full_match>();
  }

  if ( _mode & match_mode::PrefixMatchName )
  {
    return std::make_shared<prefix_match>();
  }
  
  if ( _mode & match_mode::RegexMatchName )
  {
    return std::make_shared<regex_match>();
  }
  return nullptr;
}

std::shared_ptr<imatcher> match_factory::create_value()
{
  if ( _mode & match_mode::FullMatchValue )
  {
    return std::make_shared<full_match>();
  }

  if ( _mode & match_mode::PrefixMatch )
  {
    return std::make_shared<prefix_match>();
  }
  
  if ( _mode & match_mode::RegexMatch )
  {
    return std::make_shared<regex_match>();
  }
  return nullptr;
}

}}
