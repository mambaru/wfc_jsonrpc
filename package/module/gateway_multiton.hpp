#pragma once

#include <wfc/module/component.hpp>

namespace wfc{

class gateway_multiton
  : public ::wfc::component
{
  class impl;
public:
  gateway_multiton();
};

}
