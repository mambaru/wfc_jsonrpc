
#include "conveyor_multiton.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/object.hpp>
#include "../jsonrpc/conveyor/conveyor.hpp"
#include "conveyor_config_json.hpp"

#include <wfc/name.hpp>

namespace wfc{

namespace { WFC_NAME2(conveyor_name, "jsonrpc-conveyor") }

class conveyor_multiton::impl: public ::wfc::multiton<
  conveyor_name,
  ::wfc::instance<conveyor>,
  conveyor_config_json
>
{};


conveyor_multiton::conveyor_multiton()
  : ::wfc::object( std::make_shared<conveyor_multiton::impl>() )
{}

}
