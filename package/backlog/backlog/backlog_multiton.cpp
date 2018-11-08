
#include "backlog_multiton.hpp"
#include "backlog.hpp"
#include "backlog_config_json.hpp"

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{

namespace 
{ 
  WFC_NAME2(multiton_name, "jsonrpc-backlog") 

  class impl: public ::wfc::multiton<
    multiton_name,
    ::wfc::instance<backlog>,
    backlog_config_json,
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
      return "JSONRPC notify backlog";
    }
  };
}


backlog_multiton::backlog_multiton()
  : component( std::make_shared<impl>() )
{}

}}
