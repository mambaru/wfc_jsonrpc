#pragma once

#include "statistics_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{ 
  
struct statistics_config_json
{
  JSON_NAME(target)
  JSON_NAME(time_suffix)
  JSON_NAME(size_suffix)
  JSON_NAME(request_prefix)
  JSON_NAME(notify_prefix)


  typedef json::object<
    statistics_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target, statistics_config, std::string, &statistics_config::target>,
      ::wfc::json::member<n_time_suffix, statistics_config, std::string, &statistics_config::time_suffix>,
      ::wfc::json::member<n_size_suffix, statistics_config, std::string, &statistics_config::size_suffix>,
      ::wfc::json::member<n_request_prefix, statistics_config, std::string, &statistics_config::request_prefix>,
      ::wfc::json::member<n_notify_prefix, statistics_config, std::string, &statistics_config::notify_prefix>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
