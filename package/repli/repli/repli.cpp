#include "repli.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

namespace wfc{ namespace jsonrpc{

repli::repli()
  : _counter(0)
{
}

void repli::start()
{
  this->restart();
}

void repli::restart()
{
  _counter = 0;
  std::lock_guard<mutex_type> lk(_mutex);
  _targets.clear();
  auto names = this->options().reply_targets;
  std::transform(std::begin(names), std::end(names), std::back_inserter(_targets),
                 std::bind<target_adapter>(&super::get_adapter, this, std::placeholders::_1, false));
}

void repli::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler)
{
  if ( this->suspended() )
  {
    domain_proxy::perform_incoming( std::move(holder), io_id, handler );
    return;
  }

  auto repli_holder = holder.clone(_counter);
  domain_proxy::perform_incoming( std::move(holder), io_id, handler );
  read_lock<mutex_type> lk(_mutex);
  for ( auto& r : _targets )
  {
    incoming_holder req = repli_holder.clone(_counter);
    req.parse(nullptr);
    r.perform_incoming( std::move(req), this->get_id(), [](outgoing_holder) noexcept{ });
  }
}

void repli::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( this->suspended() )
  {
    domain_proxy::perform_outgoing( std::move(holder), io_id);
    return;
  }

  auto repli_holder = holder.clone(_counter);
  domain_proxy::perform_outgoing( std::move(holder), io_id );

  read_lock<mutex_type> lk(_mutex);
  for ( auto& r : _targets )
  {
    outgoing_holder outg = repli_holder.clone(_counter);
    if ( outg.is_request() )
      outg.result_handler([](incoming_holder) noexcept {});
    r.perform_outgoing( std::move(outg), this->get_id() );
  }
}


}}
