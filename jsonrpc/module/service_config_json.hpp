#pragma once

#include "../jsonrpc/service/service_config.hpp"
#include <wfc/json.hpp>

namespace wfc{
  
struct service_config_json
{
  typedef json::object<
    service_config,
    ::wfc::json::member_list<
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
