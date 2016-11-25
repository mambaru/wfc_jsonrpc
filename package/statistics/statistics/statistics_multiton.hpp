#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace jsonrpc{ 

class statistics_multiton
  : public ::wfc::component
{
public:
  statistics_multiton();
};

}}