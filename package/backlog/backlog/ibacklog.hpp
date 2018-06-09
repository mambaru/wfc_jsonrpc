#pragma once

#include "backlog_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/workflow.hpp>

#include <string>
#include <memory>
#include <fstream>

namespace wfc{ namespace jsonrpc{
  
struct ibacklog: public iinterface
{
  virtual bool lock() = 0;
  virtual void rotate() = 0;
  virtual bool unlock() = 0;
  virtual void restore() = 0;
};

}}
