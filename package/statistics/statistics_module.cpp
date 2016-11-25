
#include "statistics_module.hpp"
#include "statistics/statistics_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{
  
namespace
{
  WFC_NAME2(module_name, "statistics")

  class impl: public ::wfc::component_list
  <
    module_name,
    statistics_multiton
  >
  {
  };
}

statistics_module::statistics_module()
  : module( std::make_shared<impl>() )
{
}

}}
