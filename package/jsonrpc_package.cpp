
#include "jsonrpc_build_info.h"

#include "jsonrpc_package.hpp"
#include "broker/broker_module.hpp"
#include "queue/queue_module.hpp"
#include "repli/repli_module.hpp"
#include "statistics/statistics_module.hpp"
#include <wfc/module/module_list.hpp>

namespace wfc{
  
class jsonrpc_package::impl: public ::wfc::module_list<
  jsonrpc_build_info,
  jsonrpc::queue_module,
  jsonrpc::broker_module,
  jsonrpc::repli_module
#ifdef WFC_ENABLE_STAT
  ,jsonrpc::statistics_module
#endif
>
{  
};

jsonrpc_package::jsonrpc_package()
  : package( std::make_shared<impl>() )
{
}

}
