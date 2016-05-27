#pragma once

#include "broker_config.hpp"
#include <wfc/json.hpp>

namespace wfc{  namespace jsonrpc{ 
  
struct broker_config_json
{
  JSON_NAME(target)
  JSON_NAME(methods)
  JSON_NAME(targets)
  JSON_NAME(reject)
  
  typedef broker_config::method_target method_target;

  typedef ::wfc::json::object<
    method_target,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target,  method_target, std::string, &method_target::target >,
      ::wfc::json::member<n_methods, method_target, std::vector<std::string>, &method_target::methods, 
        ::wfc::json::array< std::vector< ::wfc::json::value<std::string> >  > >
    >
  > method_target_json;

  
  typedef ::wfc::json::object<
    broker_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target, broker_config, std::string, &broker_config::target >,
      ::wfc::json::member<n_reject, broker_config, std::vector<std::string>, &broker_config::reject, 
        ::wfc::json::array< std::vector< ::wfc::json::value<std::string> > > >,
      ::wfc::json::member<n_targets, broker_config, std::vector<method_target>, &broker_config::targets, 
        ::wfc::json::array< std::vector< method_target_json > > >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
