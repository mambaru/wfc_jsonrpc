#pragma once

#include "../jsonrpc/conveyor/conveyor_config.hpp"
#include <wfc/json.hpp>

namespace wfc{
  
struct conveyor_config_json
{
  typedef json::object<
    conveyor_config,
    ::wfc::json::member_list<
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
