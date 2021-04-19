#pragma once

#include "repli_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/mutex.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{

class repli
  : public ::wfc::jsonrpc::domain_proxy< repli_config, ::wfc::nostat>
{
  typedef ::wfc::jsonrpc::domain_proxy< repli_config, ::wfc::nostat> super;
public:
  virtual void start() override;
  virtual void restart() override;

  // ijsonrpc

  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;

  virtual void perform_outgoing(outgoing_holder, io_id_t) override;

private:
  typedef std::vector< target_adapter > target_list;
  typedef rwlock<std::mutex> mutex_type;
  target_list _targets;
  mutex_type _mutex;
  bool _notifier = false;
};

}}
