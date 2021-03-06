
#include "queue_multiton.hpp"
#include "queue.hpp"
#include "queue_config_json.hpp"

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{

namespace 
{ 
  WFC_NAME2(multiton_name, "jsonrpc-queue") 

  class impl: public multiton<
    multiton_name,
    instance<queue>,
    queue_config_json,
      component_features::EnableTracking
    | component_features::DisabledPriority
  >
  {
    virtual std::string interface_name() const override
    {
      return std::string("wfc::ijsonrpc");
    }

    virtual std::string description() const override
    {
      return "Queue for incoming/outgoing jsonrpc messages";
    }
  };
}


queue_multiton::queue_multiton()
  : component( std::make_shared<impl>() )
{}

}}
