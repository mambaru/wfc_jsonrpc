
#include "jsonrpc_module.hpp"
#include "conveyor_multiton.hpp"
#include <wfc/module/object_list.hpp>
#include <wfc/name.hpp>

namespace wfc{
  
WFC_NAME2(jsonrpc_module_name, "jsonrpc")

class jsonrpc_module::impl: public ::wfc::object_list<
  jsonrpc_module_name,
  conveyor_multiton
>
{
};

jsonrpc_module::jsonrpc_module()
  : module( std::make_shared<jsonrpc_module::impl>() )
{
}

}
