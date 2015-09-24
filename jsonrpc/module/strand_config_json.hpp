#pragma once

#include "../jsonrpc/strand/strand_config.hpp"
#include <wfc/json.hpp>

namespace wfc{
  
struct strand_config_json
{
  typedef json::object<
    strand_config,
    ::wfc::json::member_list<
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
