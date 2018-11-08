
#include "repli_multiton.hpp"
#include "repli.hpp"
#include "repli_config_json.hpp"

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>

#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{ 

namespace 
{ 
  WFC_NAME2(multiton_name, "jsonrpc-repli") 

  class impl: public ::wfc::multiton<
    multiton_name,
    ::wfc::instance<repli>,
    repli_config_json,
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
      return "JSONRPC-notify duplicator ";
    }
  };

}


repli_multiton::repli_multiton()
  : ::wfc::component( std::make_shared<impl>() )
{}

}}
