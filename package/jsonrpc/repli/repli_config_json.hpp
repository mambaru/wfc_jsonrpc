#pragma once

#include "repli_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{ namespace repli{
  
struct repli_config_json
{
  JSON_NAME(target)
  JSON_NAME(reply_targets)

  
  typedef json::object<
    repli_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target, repli_config, std::string, &repli_config::target>,
      ::wfc::json::member<n_reply_targets, repli_config, std::vector<std::string>, &repli_config::reply_targets, vector_of_strings<10> >
      >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}}
