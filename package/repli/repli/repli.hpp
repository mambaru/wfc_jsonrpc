#pragma once

#include "repli_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{ 

class repli
  : public ::wfc::jsonrpc::domain_proxy< repli_config >
{
public:
  virtual void ready() override;

  // ijsonrpc

  virtual void perform_incoming(incoming_holder, io_id_t, rpc_outgoing_handler_t handler) override;

  virtual void perform_outgoing(outgoing_holder, io_id_t) override;

private:
  typedef std::vector< target_adapter > target_list;
  target_list _targets;
};

}}
