
#include "extract_multiton.hpp"
#include "extract.hpp"
#include "extract_config_json.hpp"

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>

#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{ 

namespace 
{ 
  WFC_NAME2(multiton_name, "jsonrpc-extract")

  class impl: public multiton<
    multiton_name,
    instance<extract>,
    extract_config_json,
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
      return "Trying to find and extract JSONRPC packages in garbage data stream";
    }
  };

}


extract_multiton::extract_multiton()
  : ::wfc::component( std::make_shared<impl>() )
{}

}}
