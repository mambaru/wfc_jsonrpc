#pragma once

#include "repli_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/mutex.hpp>
#include <string>
#include <memory>
#include <atomic>


namespace wfc{ namespace jsonrpc{

class repli
  : public ::wfc::jsonrpc::domain_proxy< repli_config, ::wfc::nostat>
{
  typedef ::wfc::jsonrpc::domain_proxy< repli_config, ::wfc::nostat> super;
public:
  repli();
  virtual void start() override;
  virtual void restart() override;

  // ijsonrpc

  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;

  virtual void perform_outgoing(outgoing_holder, io_id_t) override;

private:
  typedef std::vector< target_adapter > targets_list_t;
  typedef rwlock<std::recursive_mutex> mutex_type;
  targets_list_t _targets;
  mutex_type _mutex;
  std::atomic_int _counter;
};

}}
