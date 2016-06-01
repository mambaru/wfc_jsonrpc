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
  
  typedef ::wfc::json::object<
    queue_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target,   queue_config, std::string, &queue_config::target >,
      ::wfc::json::member<n_callback_queue, queue_config, bool, &queue_config::callback_queue>,
      ::wfc::json::member<n_callback_workflow, queue_config, std::string, &queue_config::callback_workflow >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
