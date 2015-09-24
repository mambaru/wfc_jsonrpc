#pragma once

#include <wfc/module/object.hpp>

namespace wfc{

class conveyor_multiton
  : public ::wfc::object
{
  class impl;
public:
  conveyor_multiton();
};

}
