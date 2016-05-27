
#include "jsonrpc_module.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{
  
namespace
{
  WFC_NAME2(jsonrpc_module_name, "jsonrpc")

  class impl: public ::wfc::component_list<
    jsonrpc_module_name
  >
  {
  };
}

jsonrpc_module::jsonrpc_module()
  : module( std::make_shared<impl>() )
{
}

}}
