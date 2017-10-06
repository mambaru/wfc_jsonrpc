
#include "batch_multiton.hpp"
#include "batch.hpp"
#include "batch_config_json.hpp"

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>

#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{ 

namespace 
{ 
  WFC_NAME2(multiton_name, "jsonrpc-batch") 

  class impl: public ::wfc::multiton<
    multiton_name,
    ::wfc::instance<batch>,
    batch_config_json
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


batch_multiton::batch_multiton()
  : ::wfc::component( std::make_shared<impl>() )
{}

}}
