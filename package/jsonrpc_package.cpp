
#include "jsonrpc_package.hpp"
#include "broker/broker_module.hpp"
#include "queue/queue_module.hpp"

#include "module/jsonrpc_module.hpp"
#include <wfc/module/module_list.hpp>
#include "jsonrpc_build_info.h"

namespace wfc{
  
class jsonrpc_package::impl: public ::wfc::module_list<
  jsonrpc_build_info,
  jsonrpc::broker_module,
  jsonrpc::queue_module,
  jsonrpc::jsonrpc_module
>
{  
};

jsonrpc_package::jsonrpc_package()
  : package( std::make_shared<impl>() )
{
}

}
