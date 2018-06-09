#pragma once

#include "backlog_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/workflow.hpp>

#include <string>
#include <memory>
#include <fstream>

namespace wfc{ namespace jsonrpc{
  
class backlog
  : public ::wfc::jsonrpc::domain_proxy<backlog_config, ::wfc::nostat>
{
public:
  virtual void start() override;
// ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;
  
  bool lock();
  void rotate();
  bool unlock();
  void restore();


private:
  void write_incoming_(const incoming_holder& holder);
  void apply_backlog_();
private:
  typedef std::mutex mutex_type;
  typedef std::weak_ptr<iinterface> witf;
  mutex_type _mutex;
  bool _lock_flag = false;
  std::ofstream _filelog;
  std::stringstream _ss;
};

}}
