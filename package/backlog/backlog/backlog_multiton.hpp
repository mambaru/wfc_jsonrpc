#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace jsonrpc{

class backlog_multiton
  : public ::wfc::component
{
public:
  backlog_multiton();
};

}}
