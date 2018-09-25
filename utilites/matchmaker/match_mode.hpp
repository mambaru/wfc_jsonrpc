#pragma once

namespace wfc{ namespace jsonrpc{ 

struct match_mode
{
  enum
  {
    Ignore           = 0,
    FullMatch        = 3,
    FullMatchName    = 1,
    FullMatchValue   = 2,
    PrefixMatch      = 12,
    PrefixMatchName  = 4,
    PrefixMatchValue = 8,
    RegexMatch       = 48,
    RegexMatchName   = 16,
    RegexMatchValue  = 32,
  };
};

}}
