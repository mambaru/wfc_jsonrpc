
#include "batch_module.hpp"
#include "batch/batch_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{
  
namespace
{
  WFC_NAME2(module_name, "jsonrpc-batch")

  class impl: public ::wfc::component_list
  <
    module_name,
    batch_multiton
  >
  {
  };
}

batch_module::batch_module()
  : module( std::make_shared<impl>() )
{
}

}}
