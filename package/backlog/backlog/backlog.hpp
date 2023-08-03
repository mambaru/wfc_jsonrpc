#pragma once

#include "backlog_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/workflow.hpp>

#include <string>
#include <memory>
#include <fstream>
#include <functional>

namespace wfc{ namespace jsonrpc{
  
class backlog
  : public ::wfc::jsonrpc::domain_proxy<backlog_config, ::wfc::nostat>
{
  typedef ::wfc::jsonrpc::domain_proxy<backlog_config, ::wfc::nostat> self;
public:
  backlog();
  virtual void configure() override;
  virtual void initialize() override;
  virtual void start() override;

  virtual void reg_io( io_id_t io_id, std::weak_ptr<iinterface> itf) override;

// ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;
  
  bool lock();
  void rotate();
  bool unlock();
  size_t restore();
  void async_restore(std::function<void(size_t)> cb);
  void restore_cancel();

  bool activate();
  bool deactivate();


private:
  void write_incoming_(const incoming_holder& holder);
  size_t apply_backlog_();
private:
  typedef std::mutex mutex_type;
  //typedef std::weak_ptr<iinterface> witf;
  mutex_type _mutex;
  std::string _log;
  bool _lock_flag = false;
  std::ofstream _filelog;
  std::stringstream _ss;
  std::atomic_int _counter;
  std::atomic_bool _client_mode;
  std::atomic_bool _active;
  std::atomic_bool _restore_cancel;
  wflow::owner _restore_owner;
  io_id_t _source_id = bad_id;
};

}}
