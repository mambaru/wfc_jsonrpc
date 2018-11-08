
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
    instance<hub>,
    hub_config_json,
    component_features::DisabledWorkflow
    | component_features::DisabledPriority
  >
  {
    virtual std::string interface_name() const override
    {
      return std::string("wfc::ijsonrpc");
    }

    virtual std::string description() const override
    {
      return "JSONRPC notify hub";
    }
  };
}


hub_multiton::hub_multiton()
  : component( std::make_shared<impl>() )
{}

}}
