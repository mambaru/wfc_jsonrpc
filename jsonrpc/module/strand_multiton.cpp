
#include "strand_multiton.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/object.hpp>
#include "../jsonrpc/strand/strand.hpp"
#include "strand_config_json.hpp"

#include <wfc/name.hpp>

namespace wfc{

namespace { WFC_NAME2(strand_name, "jsonrpc-strand") }

class strand_multiton::impl: public ::wfc::multiton<
  strand_name,
  ::wfc::instance<strand>,
  strand_config_json
>
{};


strand_multiton::strand_multiton()
  : ::wfc::object( std::make_shared<strand_multiton::impl>() )
{}

}
