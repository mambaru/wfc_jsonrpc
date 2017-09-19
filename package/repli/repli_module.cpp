
#include "repli_module.hpp"
#include "repli/repli_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{
  
namespace
{
  WFC_NAME2(module_name, "jsonrpc-repli")

  class impl: public ::wfc::component_list
  <
    module_name,
    repli_multiton
  >
  {
  };
}

repli_module::repli_module()
  : module( std::make_shared<impl>() )
{
}

}}
