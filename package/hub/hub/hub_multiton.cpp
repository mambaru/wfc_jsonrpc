
#include "hub_multiton.hpp"
#include "hub.hpp"
#include "hub_config_json.hpp"

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{

namespace 
{ 
  WFC_NAME2(multiton_name, "jsonrpc-hub") 

  class impl: public ::wfc::multiton<
    multiton_name,
    ::wfc::instance<hub>,
    hub_config_json,
    ::wfc::component_features::CommonWorkflow
  >
  {};
}


hub_multiton::hub_multiton()
  : component( std::make_shared<impl>() )
{}

}}
