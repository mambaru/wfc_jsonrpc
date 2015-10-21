#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace jsonrpc{ namespace repli{

class repli_multiton
  : public ::wfc::component
{
  class impl;
public:
  repli_multiton();
};

}}}
