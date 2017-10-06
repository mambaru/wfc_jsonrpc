#pragma once

#include "hub_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/workflow.hpp>

#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
class hub
  : public ::wfc::jsonrpc::domain_proxy<hub_config, ::wfc::nostat>
{
public:
// ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;
  
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override;
  virtual void unreg_io(io_id_t io_id) override;

private:
  void perform_incoming_(const incoming_holder& holder, io_id_t);
private:
  typedef std::mutex mutex_type;
  typedef std::weak_ptr<iinterface> witf;
  mutex_type _mutex;
  std::unordered_map<io_id_t, witf> _map;
};

}}
