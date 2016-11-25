
#include "statistics_multiton.hpp"
#include "statistics.hpp"
#include "statistics_config_json.hpp"

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>

#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{ 

namespace 
{ 
  WFC_NAME2(multiton_name, "jsonrpc-statistics") 

  class impl: public ::wfc::multiton<
    multiton_name,
    ::wfc::instance<statistics>,
    statistics_config_json,
      ::wfc::component_features::DisabledPriority
    | ::wfc::component_features::DisabledWorkflow

  >
  {};
}

statistics_multiton::statistics_multiton()
  : ::wfc::component( std::make_shared<impl>() )
{}

}}
