#pragma once

#include "backlog_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{
  
struct backlog_config_json
{
  JSON_NAME(target)
  JSON_NAME(path)

  typedef json::object<
    backlog_config,
    json::member_list<
      json::member<n_target,   backlog_config, std::string, &backlog_config::target >,
      json::member<n_path,   backlog_config, std::string, &backlog_config::path >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
