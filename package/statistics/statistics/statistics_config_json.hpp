#pragma once

#include "statistics_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{ 
  
struct statistics_config_json
{
  JSON_NAME(target)
  JSON_NAME(enable_write_size)
  JSON_NAME(enable_error_stat)

  typedef json::object<
    statistics_config,
    json::member_list<
      json::member<n_target, statistics_config, std::string, &statistics_config::target>,
      json::member<n_enable_write_size, statistics_config, bool, &statistics_config::enable_write_size>,
      json::member<n_enable_error_stat, statistics_config, bool, &statistics_config::enable_error_stat>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


struct statistics_stat_json
{
  JSON_NAME(time_suffix)
  JSON_NAME(read_size_suffix)
  JSON_NAME(write_size_suffix)
  JSON_NAME(request_prefix)
  JSON_NAME(notify_prefix)
  JSON_NAME(error_prefix)

  JSON_NAME(other_time)
  JSON_NAME(other_read_size)
  JSON_NAME(other_write_size)

  typedef json::object<
    statistics_stat,
    json::member_list<
      json::member<n_time_suffix, statistics_stat, std::string, &statistics_stat::time_suffix>,
      json::member<n_read_size_suffix, statistics_stat, std::string, &statistics_stat::read_size_suffix>,
      json::member<n_write_size_suffix, statistics_stat, std::string, &statistics_stat::write_size_suffix>,
      json::member<n_request_prefix, statistics_stat, std::string, &statistics_stat::request_prefix>,
      json::member<n_notify_prefix, statistics_stat, std::string, &statistics_stat::notify_prefix>,
      json::member<n_error_prefix, statistics_stat, std::string, &statistics_stat::error_prefix>,
      json::member<n_other_time, statistics_stat, std::string, &statistics_stat::other_time>,
      json::member<n_other_read_size, statistics_stat, std::string, &statistics_stat::other_read_size>,
      json::member<n_other_write_size, statistics_stat, std::string, &statistics_stat::other_write_size>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
