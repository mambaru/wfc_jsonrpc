#pragma once

#include "stub_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/mutex.hpp>
#include <string>
#include <memory>
#include <atomic>

namespace wfc{ namespace jsonrpc{

class stub
  : public domain_proxy< stub_config, nostat>
  , private std::enable_shared_from_this<stub>
{
  typedef domain_proxy< stub_config, nostat> super;
  class stab_handler;

public:
  stub();
  virtual void reg_io( io_id_t io_id, std::weak_ptr<iinterface> itf) override;
  virtual void unreg_io(io_id_t io_id) override;

  virtual void start() override;
  virtual void restart() override;

  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;

  virtual void perform_outgoing(outgoing_holder, io_id_t) override;

private:
  typedef std::vector< target_adapter > target_list;
  typedef rwlock<std::mutex> mutex_type;
  target_list _targets;
  mutex_type _mutex;
  stub_config _conf;
  std::atomic_int _counter;
  std::map<io_id_t, std::shared_ptr<stab_handler> > _handler_map;
};

}}
