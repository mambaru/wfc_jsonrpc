#pragma once

#include "workflow_config.hpp"
#include <wfc/json.hpp>
#include <wfc/core/workflow_options_json.hpp>

namespace wfc{ namespace jsonrpc{
  
 
struct workflow_config_json
{
  JSON_NAME(target)
  JSON_NAME(incoming)
  JSON_NAME(incoming_disabled)
  JSON_NAME(outgoing)
  JSON_NAME(outgoing_disabled)
  
  typedef ::wfc::json::object<
    workflow_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target,   workflow_config, std::string, &workflow_config::target >,
      
      ::wfc::json::member<n_incoming_disabled, workflow_config, bool, &workflow_config::incoming_disabled>,
      ::wfc::json::member<n_incoming, workflow_config, ::wfc::workflow_options, &workflow_config::incoming, ::wfc::workflow_options2_json >,
      
      ::wfc::json::member<n_outgoing_disabled, workflow_config, bool, &workflow_config::outgoing_disabled>,
      ::wfc::json::member<n_outgoing, workflow_config, ::wfc::workflow_options, &workflow_config::outgoing, ::wfc::workflow_options2_json >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
