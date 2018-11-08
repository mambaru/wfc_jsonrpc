
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

  class impl: public multiton<
    multiton_name,
    instance<statistics>,
    statistics_config_json,
      component_features::DisabledPriority
    | component_features::DisabledWorkflow,
    statistics_stat_json
  >
  {};
}

statistics_multiton::statistics_multiton()
  : component( std::make_shared<impl>() )
{}

}}
