#pragma once

#include <wfc/module/object.hpp>

namespace wfc{

class strand_multiton
  : public ::wfc::object
{
  class impl;
public:
  strand_multiton();
};

}
