#pragma once

#include <wfc/module/object.hpp>

namespace wfc{

class broker_multiton
  : public ::wfc::object
{
  class impl;
public:
  broker_multiton();
};

}
