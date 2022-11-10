#pragma once

#include "stub_config.hpp"
#include <wjson/wjson.hpp>

namespace wfc{ namespace jsonrpc{



struct stub_config_json
{
  struct mode_json
  {
    JSON_NAME(forward_response)
    JSON_NAME(ignore_errors)
    JSON_NAME(fake_response)
    JSON_NAME(advance_response)

    typedef wjson::enumerator<
      stub_config::mode_t,
      wjson::member_list<
        wjson::enum_value<n_forward_response, stub_config::mode_t, stub_config::forward_response>,
        wjson::enum_value<n_ignore_errors, stub_config::mode_t, stub_config::ignore_errors>,
        wjson::enum_value<n_fake_response, stub_config::mode_t, stub_config::fake_response>,
        wjson::enum_value<n_advance_response, stub_config::mode_t, stub_config::advance_response>
      >
    > meta;

    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;

  };
  JSON_NAME(target)
  JSON_NAME(to_notify)
  JSON_NAME(to_request)
  JSON_NAME(mode)
  JSON_NAME(fake_result)

  typedef wjson::object<
    stub_config,
    wjson::member_list<
      wjson::member<n_target, stub_config, std::string, &stub_config::target>,
      wjson::member<n_to_notify, stub_config, bool, &stub_config::to_notify>,
      wjson::member<n_to_request, stub_config, bool, &stub_config::to_request>,
      wjson::member<n_mode, stub_config, stub_config::mode_t, &stub_config::mode, mode_json>,
/*      wjson::member<n_fix_errors, stub_config, bool, &stub_config::fix_errors>,
      wjson::member<n_advance_response, stub_config, bool, &stub_config::advance_response>,
      */
      wjson::member<n_fake_result, stub_config, std::shared_ptr<std::string>, &stub_config::fake_result,
        wjson::pointer< std::shared_ptr<std::string>, wjson::raw_value< std::string> >
      >
    >
  > meta;

  typedef meta::target target;
  typedef meta::serializer serializer;
  typedef meta::member_list member_list;
};

}}
