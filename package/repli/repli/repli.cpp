#include "repli.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

namespace wfc{ namespace jsonrpc{

void repli::start()
{
  this->restart();
}

void repli::restart()
{
  std::lock_guard<mutex_type> lk(_mutex);
  _targets.clear();
  _notifier = this->options().notifier;
  auto names = this->options().reply_targets;
  std::transform(std::begin(names), std::end(names), std::back_inserter(_targets),
                 std::bind<target_adapter>(&super::get_adapter, this, std::placeholders::_1, false));
}

void repli::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler)
{
  if ( !this->suspended() )
  {
    read_lock<mutex_type> lk(_mutex);
    for ( auto& r : _targets )
    {
      if ( holder.is_notify() || _notifier )
        r.perform_incoming( holder.clone(), this->get_id(), nullptr);
      else
        r.perform_incoming( holder.clone(), this->get_id(), [](outgoing_holder){});
    }
  }
  domain_proxy::perform_incoming( std::move(holder), io_id, handler );
}

void repli::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( !this->suspended()  )
  {
    read_lock<mutex_type> lk(_mutex);
    for ( auto& r : _targets )
    {
      if ( _notifier || holder.is_notify() )
        r.perform_outgoing( holder.clone(), this->get_id() );
      else
      {
        auto req = holder.clone(1);
        req.result_handler([](incoming_holder){});
        r.perform_outgoing( std::move(req), this->get_id() );
      }
    }
  }
  domain_proxy::perform_outgoing( std::move(holder), io_id);
}


}}
