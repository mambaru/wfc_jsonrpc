
#include "gateway_multiton.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/object.hpp>
#include "../jsonrpc/gateway/gateway.hpp"
#include "gateway_config_json.hpp"

#include <wfc/name.hpp>

namespace wfc{

namespace { WFC_NAME2(gateway_name, "jsonrpc-gateway") }

class gateway_multiton::impl: public ::wfc::multiton<
  gateway_name,
  ::wfc::instance<gateway>,
  gateway_config_json
>
{};


gateway_multiton::gateway_multiton()
  : ::wfc::object( std::make_shared<gateway_multiton::impl>() )
{}

}
