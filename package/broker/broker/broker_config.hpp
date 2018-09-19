#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include <regex>
#if __cplusplus >= 201103L &&                             \
    (!defined(__GLIBCXX__) || (__cplusplus >= 201402L) || \
        (defined(_GLIBCXX_REGEX_DFS_QUANTIFIERS_LIMIT) || \
         defined(_GLIBCXX_REGEX_STATE_LIMIT)           || \
             (defined(_GLIBCXX_RELEASE)                && \
             _GLIBCXX_RELEASE > 4)))
#define HAVE_WORKING_REGEX 1
#else
#define HAVE_WORKING_REGEX 0
#endif
    

namespace wfc{ namespace jsonrpc{ 

struct broker_config
{
  /*
#ifdef HAVE_WORKING_REGEX 
  struct param
  {
    std::string regex;
    std::map<std::string, std::shared_ptr<std::string> > fields;
  };
#endif
  */
  
  struct rule
  {
    std::string target;
    std::set<std::string> methods;
#ifdef HAVE_WORKING_REGEX 
    std::map<std::string, std::shared_ptr<std::string> > params;
#endif
    
  };
  
  std::string target;
  std::vector<rule> rules;
  std::vector<std::string> reject;
};

} }
