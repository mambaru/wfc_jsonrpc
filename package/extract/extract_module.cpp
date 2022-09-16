
#include "extract_module.hpp"
#include "extract/extract_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{
  
namespace
{
  WFC_NAME2(module_name, "jsonrpc-extract")

  class impl: public ::wfc::component_list
  <
    module_name,
    extract_multiton
  >
  {
  };
}

extract_module::extract_module()
  : module( std::make_shared<impl>() )
{
}

}}
