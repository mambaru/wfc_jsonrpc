#include "backlog/backlog_multiton.hpp"
#include "backlog_module.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{
  
namespace
{
  WFC_NAME2(module_name, "jsonrpc-backlog")

  class impl: public ::wfc::component_list<
    module_name,
    backlog_multiton
  >
  {};
}

backlog_module::backlog_module()
  : module( std::make_shared<impl>() )
{
}

}}
