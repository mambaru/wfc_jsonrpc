#pragma once

#include "broker_config.hpp"
#include <wfc/json.hpp>

namespace wfc{  namespace jsonrpc{ 
 
/*
 * struct match_mode
{
  enum
  {
    FullMatch        = 3,
    FullMatchName    = 1,
    FullMatchValue   = 2,
    PrefixMatch      = 12,
    PrefixMatchName  = 4,
    PrefixMatchValue = 8,
    RegexMatch       = 48,
    RegexMatchName   = 16,
    RegexMatchValue  = 32,
  };
};

 */
struct broker_config_json
{
  JSON_NAME(FullMatch)
  JSON_NAME(FullMatchName)
  JSON_NAME(FullMatchValue)
  JSON_NAME(PrefixMatch)
  JSON_NAME(PrefixMatchName)
  JSON_NAME(PrefixMatchValue)
  JSON_NAME(RegexMatch)
  JSON_NAME(RegexMatchName)
  JSON_NAME(RegexMatchValue)

  typedef json::flags<
    int,
    json::member_list<
      json::enum_value<n_FullMatch, int, match_mode::FullMatch>,
      json::enum_value<n_FullMatchName, int, match_mode::FullMatchName>,
      json::enum_value<n_FullMatchValue, int, match_mode::FullMatchValue>,
      json::enum_value<n_PrefixMatch, int, match_mode::PrefixMatch>,
      json::enum_value<n_PrefixMatchName, int, match_mode::PrefixMatchName>,
      json::enum_value<n_PrefixMatchValue, int, match_mode::PrefixMatchValue>,
      json::enum_value<n_RegexMatch, int, match_mode::RegexMatch>,
      json::enum_value<n_RegexMatchName, int, match_mode::RegexMatchName>,
      json::enum_value<n_RegexMatchValue, int, match_mode::RegexMatchValue>
    >, '|'
  > match_mode_json;
  
  typedef broker_config::rule rule;
  typedef std::shared_ptr<std::string> string_ptr_t;
  typedef json::pointer< string_ptr_t, json::string<> > string_ptr_json;

  JSON_NAME(methods)
  JSON_NAME(target)
  JSON_NAME(params)
  JSON_NAME(mode)

  typedef ::wfc::json::object<
    rule,
    json::member_list<
      json::member<n_methods, rule, std::set<std::string>, &rule::methods, json::set_of< json::string<> > >,
      json::member<n_target,  rule, string_ptr_t, &rule::target, string_ptr_json >,
      json::member<n_params,  rule, string_ptr_t, &rule::params, string_ptr_json >,
      json::member<n_mode,    rule, int, &rule::mode, match_mode_json >
    >,
    json::strict_mode
  > rule_json;

  typedef json::array< std::vector< rule_json > > array_rule_json;
  JSON_NAME(rules)
  JSON_NAME(reject)

  
  typedef json::object<
    broker_config,
    json::member_list<
      json::member<n_target, broker_config, std::string, &broker_config::target >,
      json::member<n_reject, broker_config, std::vector<std::string>, &broker_config::reject, json::vector_of_strings<> >,
      json::member<n_rules, broker_config, std::vector<rule>, &broker_config::rules, array_rule_json >
    >,
    json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
