#pragma once

#include "queue_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/workflow.hpp>

#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
class queue
  : public domain_proxy<queue_config, nostat>
  , public std::enable_shared_from_this<queue>
{
  typedef domain_proxy<queue_config, nostat> super;
  typedef std::shared_ptr< workflow > workflow_ptr;
public:
  queue();
// domain
  virtual void restart() override;
  virtual void stop() override;
// ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;
  
private:
  std::function<void()> make_post_fun_(const std::shared_ptr<incoming_holder>& pholder, io_id_t io_id, outgoing_handler_t handler);
  std::function<void()> make_track_fun_(io_id_t io_id, std::function<void()> fun);
  outgoing_handler_t make_outgoing_handler_(outgoing_handler_t handler);
  void drop_handler_(std::shared_ptr<incoming_holder> pholder, outgoing_handler_t handler);
private:
  typedef std::mutex mutex_type;
  workflow_ptr _callback_workflow;
  //std::atomic<bool> _connection_tracking;
  //std::mutex _tracking_mutex;
  //std::map<io_id_t, std::shared_ptr<size_t> > _tracking_map;
};

}}
