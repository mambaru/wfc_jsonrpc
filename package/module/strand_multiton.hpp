#pragma once

#include <wfc/module/component.hpp>

namespace wfc{

class strand_multiton
  : public ::wfc::component
{
  class impl;
public:
  strand_multiton();
};

}
