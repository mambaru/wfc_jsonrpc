#pragma once

#include "../jsonrpc/gateway/gateway_config.hpp"
#include <wfc/json.hpp>

namespace wfc{
  
struct gateway_config_json
{
  typedef json::object<
    gateway_config,
    ::wfc::json::member_list<
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
