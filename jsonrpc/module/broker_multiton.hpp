#pragma once

#include <wfc/module/component.hpp>

namespace wfc{

class broker_multiton
  : public ::wfc::component
{
  class impl;
public:
  broker_multiton();
};

}
