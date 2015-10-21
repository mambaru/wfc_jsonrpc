
#include "jsonrpc_module.hpp"
#include "../jsonrpc/repli/repli_multiton.hpp"
#include "gateway_multiton.hpp"
#include "broker_multiton.hpp"
#include "strand_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{
  
WFC_NAME2(jsonrpc_module_name, "jsonrpc")

class jsonrpc_module::impl: public ::wfc::component_list<
  jsonrpc_module_name,
  repli::repli_multiton,
  gateway_multiton,
  broker_multiton,
  strand_multiton
>
{
  
};

jsonrpc_module::jsonrpc_module()
  : module( std::make_shared<jsonrpc_module::impl>() )
{
}

}}
