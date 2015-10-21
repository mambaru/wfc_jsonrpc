
#include "repli_multiton.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>
#include "repli.hpp"
#include "repli_config_json.hpp"

#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{ namespace repli{

namespace { WFC_NAME2(multiton_name, "jsonrpc-repli") }

class repli_multiton::impl: public ::wfc::multiton<
  multiton_name,
  ::wfc::instance<repli>,
  repli_config_json
>
{};


repli_multiton::repli_multiton()
  : ::wfc::component( std::make_shared<repli_multiton::impl>() )
{}

}}}
