#pragma once

#include <wfc/module/component.hpp>

namespace wfc{

class service_multiton
  : public ::wfc::component
{
  class impl;
public:
  service_multiton();
};

}
