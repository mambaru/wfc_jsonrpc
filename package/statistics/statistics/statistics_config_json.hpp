#pragma once

#include "statistics_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{ 
  
struct statistics_config_json
{
  JSON_NAME(target)

  typedef json::object<
    statistics_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target, statistics_config, std::string, &statistics_config::target>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
