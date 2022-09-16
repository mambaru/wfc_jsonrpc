#pragma once

#include "extract_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{ 
  
struct extract_config_json
{
  JSON_NAME(target)
  JSON_NAME(stream_mode)
  JSON_NAME(show_parse_errors)
  
  typedef json::object<
    extract_config,
    json::member_list<
      json::member<n_target, extract_config, std::string, &extract_config::target>,
      json::member<n_stream_mode, extract_config, bool, &extract_config::stream_mode>,
      json::member<n_show_parse_errors, extract_config, bool, &extract_config::show_parse_errors>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
