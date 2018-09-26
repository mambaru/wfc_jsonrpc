
#include "match_factory.hpp"
#include "../match_mode.hpp"

#include "full_match.hpp"
#include "prefix_match.hpp"
#include "regex_match.hpp"
namespace wfc{ namespace jsonrpc{ 

match_factory::match_factory(int mode)
  : _mode(mode)
{}

std::shared_ptr<imatcher> match_factory::create()
{
  if ( _mode & match_mode::FullMatch )
    return std::make_shared<full_match>();

  if ( _mode & match_mode::PrefixMatch )
    return std::make_shared<prefix_match>();
  
  if ( _mode & match_mode::RegexMatch )
    return std::make_shared<regex_match>();
  
  return nullptr;
}

}}
