#pragma once

#include "backlog_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{
  
struct backlog_config_json
{
  JSON_NAME(restore_target)
  JSON_NAME(target)
  JSON_NAME(path)
  JSON_NAME(restore_trace)
  JSON_NAME(log)

  typedef json::object<
    backlog_config,
    json::member_list<
      json::member<n_target,   backlog_config, std::string, &backlog_config::target >,
      json::member<n_restore_target,   backlog_config, std::string, &backlog_config::restore_target >,
      json::member<n_path,   backlog_config, std::string, &backlog_config::path >,
      json::member<n_restore_trace,   backlog_config, size_t, &backlog_config::restore_trace >,
      json::member<n_log,   backlog_config, std::string, &backlog_config::log >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
