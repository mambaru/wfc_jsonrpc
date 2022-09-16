
#include "jsonrpc_build_info.h"

#include "jsonrpc_package.hpp"
#include "broker/broker_module.hpp"
#include "queue/queue_module.hpp"
#include "repli/repli_module.hpp"
#include "hub/hub_module.hpp"
#include "backlog/backlog_module.hpp"
#include "batch/batch_module.hpp"
#include "extract/extract_module.hpp"
#include "statistics/statistics_module.hpp"
#include <wfc/module/module_list.hpp>

namespace wfc{

class jsonrpc_package::impl: public ::wfc::module_list<
  jsonrpc_build_info,
  jsonrpc::broker_module,
  jsonrpc::queue_module,
  jsonrpc::repli_module,
  jsonrpc::hub_module,
  jsonrpc::batch_module,
  jsonrpc::extract_module,
  jsonrpc::backlog_module
#ifdef WFC_ENABLE_STAT
  ,jsonrpc::statistics_module
#endif
>
{
  virtual std::string description() const override
  {
    return "Additional jsonrpc modules";
  }

  virtual int show_order() const override
  {
    return 2000;
  }
};

jsonrpc_package::jsonrpc_package()
  : package( std::make_shared<impl>() )
{
}

}
