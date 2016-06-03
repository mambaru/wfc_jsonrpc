#pragma once

#include "hub_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/workflow.hpp>

#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
class hub
  : public ::wfc::jsonrpc::domain_proxy<hub_config>
{
public:
  hub();
// domain
  virtual void ready() override;

  /*
  virtual void reg_io(io_id_t , std::weak_ptr<iinterface> );

  virtual void unreg_io(io_id_t );

  virtual void perform_io(data_ptr , io_id_t , outgoing_handler_t handler);
  */
// ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, rpc_outgoing_handler_t handler) override;
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;
  
private:
};

}}
