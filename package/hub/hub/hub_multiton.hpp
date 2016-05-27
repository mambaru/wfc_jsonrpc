#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace jsonrpc{

class hub_multiton
  : public ::wfc::component
{
public:
  hub_multiton();
};

}}
