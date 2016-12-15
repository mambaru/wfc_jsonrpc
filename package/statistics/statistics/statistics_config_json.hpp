#pragma once

#include "statistics_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{ 
  
struct statistics_config_json
{
  JSON_NAME(target)
  JSON_NAME(enable_write_size)
  JSON_NAME(enable_error_stat)
  JSON_NAME(time_suffix)
  JSON_NAME(read_size_suffix)
  JSON_NAME(write_size_suffix)
  JSON_NAME(request_prefix)
  JSON_NAME(notify_prefix)

  JSON_NAME(other_time)
  JSON_NAME(other_read_size)
  JSON_NAME(other_write_size)


  typedef json::object<
    statistics_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target, statistics_config, std::string, &statistics_config::target>,
      ::wfc::json::member<n_enable_write_size, statistics_config, bool, &statistics_config::enable_write_size>,
      ::wfc::json::member<n_enable_error_stat, statistics_config, bool, &statistics_config::enable_error_stat>,
      ::wfc::json::member<n_time_suffix, statistics_config, std::string, &statistics_config::time_suffix>,
      ::wfc::json::member<n_read_size_suffix, statistics_config, std::string, &statistics_config::read_size_suffix>,
      ::wfc::json::member<n_write_size_suffix, statistics_config, std::string, &statistics_config::write_size_suffix>,
      ::wfc::json::member<n_request_prefix, statistics_config, std::string, &statistics_config::request_prefix>,
      ::wfc::json::member<n_notify_prefix, statistics_config, std::string, &statistics_config::notify_prefix>,
      ::wfc::json::member<n_other_time, statistics_config, std::string, &statistics_config::other_time>,
      ::wfc::json::member<n_other_read_size, statistics_config, std::string, &statistics_config::other_read_size>,
      ::wfc::json::member<n_other_write_size, statistics_config, std::string, &statistics_config::other_write_size>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
