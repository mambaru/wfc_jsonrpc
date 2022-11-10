
#include "stub_module.hpp"
#include "stub/stub_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{
  
namespace
{
  WFC_NAME2(module_name, "jsonrpc-stub")

  class impl: public ::wfc::component_list
  <
    module_name,
    stub_multiton
  >
  {
  };
}

stub_module::stub_module()
  : module( std::make_shared<impl>() )
{
}

}}
