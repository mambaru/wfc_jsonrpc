#pragma once

#include "hub_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{
  
 
struct hub_config_json
{
    JSON_NAME(target)

  typedef ::wfc::json::object<
    hub_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target,   hub_config, std::string, &hub_config::target >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
