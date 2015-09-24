
#include "jsonrpc_module.hpp"
#include "service_multiton.hpp"
#include "gateway_multiton.hpp"
#include "broker_multiton.hpp"
#include "strand_multiton.hpp"
#include <wfc/module/object_list.hpp>
#include <wfc/name.hpp>

namespace wfc{
  
WFC_NAME2(jsonrpc_module_name, "jsonrpc")

class jsonrpc_module::impl: public ::wfc::object_list<
  jsonrpc_module_name,
  service_multiton,
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

}
