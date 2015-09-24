#pragma once

#include <wfc/module/module.hpp>

namespace wfc{

class jsonrpc_module
  : public ::wfc::module
{
  class impl;
public:
  jsonrpc_module();
};

}
