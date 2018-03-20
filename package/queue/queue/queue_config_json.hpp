#pragma once

#include "queue_config.hpp"
#include <wfc/json.hpp>
#include <wfc/core/workflow_options_json.hpp>

namespace wfc{ namespace jsonrpc{
  
 
struct queue_config_json
{
  JSON_NAME(target)
  JSON_NAME(callback_workflow)
  JSON_NAME(callback_queue)
  JSON_NAME(connection_tracking)
  
  typedef json::object<
    queue_config,
    json::member_list<
      json::member<n_target,   queue_config, std::string, &queue_config::target >,
      json::member<n_callback_queue, queue_config, bool, &queue_config::callback_queue>,
      json::member<n_connection_tracking, queue_config, bool, &queue_config::connection_tracking>,
      json::member<n_callback_workflow, queue_config, std::string, &queue_config::callback_workflow >
    >,
    json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
