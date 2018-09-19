#pragma once

#include "broker_config.hpp"
#include <wfc/json.hpp>

namespace wfc{  namespace jsonrpc{ 
  
struct broker_config_json
{
  JSON_NAME(target)
  JSON_NAME(methods)
  JSON_NAME(rules)
  JSON_NAME(reject)
  
  typedef broker_config::rule rule;

  typedef ::wfc::json::object<
    rule,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target,  rule, std::string, &rule::target >,
      ::wfc::json::member<n_methods, rule, std::set<std::string>, &rule::methods, json::set_of< json::string<>  > >
    >
  > rule_json;

  
  typedef ::wfc::json::object<
    broker_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target, broker_config, std::string, &broker_config::target >,
      ::wfc::json::member<n_reject, broker_config, std::vector<std::string>, &broker_config::reject, 
        /*::wfc::json::array< std::vector< ::wfc::json::value<std::string> > >*/ ::wfc::json::vector_of_strings<10> >,
      ::wfc::json::member<n_rules, broker_config, std::vector<rule>, &broker_config::rules, 
        ::wfc::json::array< std::vector< rule_json > > >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
