
#include "jsonrpc_package.hpp"
#include "module/jsonrpc_module.hpp"
#include <wfc/module/module_list.hpp>
#include "jsonrpc_build_info.h"

namespace wfc{
  
class jsonrpc_package::impl: public ::wfc::module_list<
  jsonrpc_build_info,
  jsonrpc::jsonrpc_module
>
{  
};

jsonrpc_package::jsonrpc_package()
  : package( std::make_shared<impl>() )
{
}

}
