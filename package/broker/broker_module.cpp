#include "broker/broker_multiton.hpp"
#include "broker_module.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{
  
namespace
{
  WFC_NAME2(module_name, "broker")

  class impl: public ::wfc::component_list<
    module_name,
    broker_multiton
  >
  {
  };
}

broker_module::broker_module()
  : module( std::make_shared<impl>() )
{
}

}}
