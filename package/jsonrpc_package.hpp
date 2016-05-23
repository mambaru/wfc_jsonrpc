#pragma once

#include <wfc/module/package.hpp>

namespace wfc{

class jsonrpc_package
  : public ::wfc::package
{
  class impl;
public:
  jsonrpc_package();
};

}
