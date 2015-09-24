#pragma once

#include "../jsonrpc/broker/broker_config.hpp"
#include <wfc/json.hpp>

namespace wfc{
  
struct broker_config_json
{
  typedef json::object<
    broker_config,
    ::wfc::json::member_list<
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
