
#include "broker_multiton.hpp"
#include "broker.hpp"
#include "broker_config_json.hpp"


#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>
#include <wfc/name.hpp>

namespace wfc{

namespace { WFC_NAME2(broker_name, "jsonrpc-broker") }

class broker_multiton::impl: public ::wfc::multiton<
  broker_name,
  ::wfc::instance<broker>,
  broker_config_json
>
{};


broker_multiton::broker_multiton()
  : ::wfc::component( std::make_shared<broker_multiton::impl>() )
{}

}
