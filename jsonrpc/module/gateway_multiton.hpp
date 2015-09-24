#pragma once

#include <wfc/module/object.hpp>

namespace wfc{

class gateway_multiton
  : public ::wfc::object
{
  class impl;
public:
  gateway_multiton();
};

}
