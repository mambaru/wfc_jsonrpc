#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace jsonrpc{ 

class stub_multiton
  : public ::wfc::component
{
public:
  stub_multiton();
};

}}
