#pragma once

#include "batch_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{ 
  
struct batch_config_json
{
  JSON_NAME(target)
  
  typedef json::object<
    batch_config,
    json::member_list<
      json::member<n_target, batch_config, std::string, &batch_config::target>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
