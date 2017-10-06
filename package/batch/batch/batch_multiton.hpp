#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace jsonrpc{ 

class batch_multiton
  : public ::wfc::component
{
public:
  batch_multiton();
};

}}