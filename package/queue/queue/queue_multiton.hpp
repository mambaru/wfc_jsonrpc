#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace jsonrpc{

class queue_multiton
  : public ::wfc::component
{
public:
  queue_multiton();
};

}}
