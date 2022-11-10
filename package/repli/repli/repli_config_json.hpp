#pragma once

#include "repli_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{

struct repli_config_json
{
  JSON_NAME(target)
  JSON_NAME(reply_targets)


  typedef json::object<
    repli_config,
    wjson::member_list<
      wjson::member<n_target, repli_config, std::string, &repli_config::target>,
      wjson::member<n_reply_targets, repli_config, std::vector<std::string>, &repli_config::reply_targets, wjson::vector_of_strings<10> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
