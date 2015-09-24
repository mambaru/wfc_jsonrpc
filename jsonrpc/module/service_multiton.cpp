
#include "service_multiton.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/object.hpp>
#include "../jsonrpc/service/service.hpp"
#include "service_config_json.hpp"

#include <wfc/name.hpp>

namespace wfc{

namespace { WFC_NAME2(service_name, "jsonrpc-service") }

class service_multiton::impl: public ::wfc::multiton<
  service_name,
  ::wfc::instance<service>,
  service_config_json
>
{};


service_multiton::service_multiton()
  : ::wfc::object( std::make_shared<service_multiton::impl>() )
{}

}
