#pragma once

#include "../jsonrpc/strand/strand_config.hpp"
#include <iow/io/strand/mtoptions_json.hpp>
#include <wfc/json.hpp>

namespace wfc{
  
struct strand_config_json
{
  typedef ::iow::io::strand::mtoptions_json mtoptions_json;
  typedef strand_config::strand_options strand_options;
  
  JSON_NAME(target)
  JSON_NAME(incoming)
  JSON_NAME(outgoing)
  
  typedef json::object<
    strand_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target,   strand_config, std::string,    &strand_config::target>,
      ::wfc::json::member<n_incoming, strand_config, strand_options, &strand_config::incoming, mtoptions_json>,
      ::wfc::json::member<n_outgoing, strand_config, strand_options, &strand_config::outgoing, mtoptions_json>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;

  /*
  JSON_NAME(disabled)
  JSON_NAME(target)
  JSON_NAME(maxsize)
  JSON_NAME(wrnsize)
  JSON_NAME(strands)
  JSON_NAME(threads)
  
  typedef json::object<
    strand_options,
    ::wfc::json::member_list<
      ::wfc::json::member<n_disabled, strand_options, bool, &strand_options::disabled>, 
      ::wfc::json::member<n_maxsize, strand_options, size_t, &strand_options::maxsize>,
      ::wfc::json::member<n_wrnsize, strand_options, size_t, &strand_options::wrnsize>,
      ::wfc::json::member<n_strands, strand_options, size_t, &strand_options::strands>,
      ::wfc::json::member<n_threads, strand_options, size_t, &strand_options::threads>
    >
  > part_json;

  JSON_NAME(incoming)
  JSON_NAME(outgoing)
  
  typedef json::object<
    strand_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_target, strand_config, std::string, &strand_config::target>,
      ::wfc::json::member<n_incoming, strand_config, strand_options, &strand_config::incoming, part_json>,
      ::wfc::json::member<n_outgoing, strand_config, strand_options, &strand_config::outgoing, part_json>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
  */
};

}
