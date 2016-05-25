#pragma once

#include "queue_config.hpp"
#include <wfc/json.hpp>
#include <wfc/core/workflow_options_json.hpp>

namespace wfc{ namespace jsonrpc{
  
 
struct queue_config_json
{
  JSON_NAME(target)
  JSON_NAME(incoming)
  JSON_NAME(incoming_disabled)
  JSON_NAME(outgoing)
  JSON_NAME(outgoing_disabled)
  
  typedef ::wfc::json::object<
    queue_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target,   queue_config, std::string, &queue_config::target >,
      
      ::wfc::json::member<n_incoming_disabled, queue_config, bool, &queue_config::incoming_disabled>,
      ::wfc::json::member<n_incoming, queue_config, ::wfc::workflow_options, &queue_config::incoming, ::wfc::workflow_options2_json >,
      
      ::wfc::json::member<n_outgoing_disabled, queue_config, bool, &queue_config::outgoing_disabled>,
      ::wfc::json::member<n_outgoing, queue_config, ::wfc::workflow_options, &queue_config::outgoing, ::wfc::workflow_options2_json >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
