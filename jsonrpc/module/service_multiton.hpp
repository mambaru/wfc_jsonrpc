#pragma once

#include <wfc/module/object.hpp>

namespace wfc{

class service_multiton
  : public ::wfc::object
{
  class impl;
public:
  service_multiton();
};

}
