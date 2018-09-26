
#include "broker_multiton.hpp"
#include "broker.hpp"
#include "broker_config_json.hpp"


#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{ 

namespace { 
  
  WFC_NAME2(component_name, "jsonrpc-broker") 

  class impl: public ::wfc::multiton<
    component_name,
    ::wfc::instance<broker>,
    broker_config_json,
    component_features::DisabledWorkflow | component_features::DisabledPriority
  >
  {
    virtual std::string interface_name() const override
    {
      return std::string("wfc::ijsonrpc");
    }

    virtual std::string description() const override
    {
      return "JSONRPC broker of methods";
    }
  };

}


broker_multiton::broker_multiton()
  : ::wfc::component( std::make_shared<impl>() )
{}

}}
