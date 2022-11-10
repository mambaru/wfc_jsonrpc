#pragma once

#include <wfc/module/module.hpp>

namespace wfc{ namespace jsonrpc{

class stub_module
  : public ::wfc::module
{
public:
  stub_module();
};

}}
