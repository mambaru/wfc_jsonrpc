#include "domain/queue_multiton.hpp"
#include "queue_module.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{
  
namespace
{
  WFC_NAME2(module_name, "jsonrpc-queue")

  class impl: public ::wfc::component_list<
    module_name,
    queue_multiton
  >
  {};
}

queue_module::queue_module()
  : module( std::make_shared<impl>() )
{
}

}}
