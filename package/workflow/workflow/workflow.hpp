#pragma once

#include "workflow_config.hpp"
#include <domain_base.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/workflow.hpp>

#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
class workflow
  : public domain_base<workflow_config>
{
  typedef std::shared_ptr< ::wfc::workflow > workflow_ptr;
public:
  workflow();
// domain
  virtual void reconfigure() override;
  virtual void stop(const std::string&) override;

// ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, rpc_outgoing_handler_t handler) override;
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;
  
private:
  rpc_outgoing_handler_t make_handler_(rpc_outgoing_handler_t&& handler);
private:
  workflow_ptr _incoming;
  workflow_ptr _outgoing;
};

}}
