#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace jsonrpc{ 

class repli_multiton
  : public ::wfc::component
{
public:
  repli_multiton();
};

}}