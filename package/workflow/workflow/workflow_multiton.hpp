#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace jsonrpc{

class workflow_multiton
  : public ::wfc::component
{
public:
  workflow_multiton();
};

}}
