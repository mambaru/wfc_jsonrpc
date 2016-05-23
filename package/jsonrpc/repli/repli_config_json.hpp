#pragma once

#include "repli_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{ namespace repli{
  
struct repli_config_json
{
  JSON_NAME(primary_target)
  JSON_NAME(targets)

  
  typedef json::object<
    repli_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_primary_target, repli_config, std::string, &repli_config::primary_target>,
      ::wfc::json::member<n_targets, repli_config, std::vector<std::string>, &repli_config::targets
                        , ::wfc::json::array< std::vector< ::wfc::json::value< std::string > > >
      >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}}
