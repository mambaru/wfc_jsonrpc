#include "repli.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <wfc/jsonrpc/target.hpp>

namespace wfc{ namespace jsonrpc{ 

void repli::restart()
{
  std::lock_guard<mutex_type> lk(_mutex);
  _targets.clear();
  for ( auto rname : this->options().reply_targets )
  {
    _targets.push_back(  this->get_adapter(rname) );
  }
}

void repli::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
{
  if ( !this->suspended() )
  {
    read_lock<mutex_type> lk(_mutex);
    for ( auto& r : _targets )
      r.perform_incoming( holder.clone(), io_id, nullptr);
  }
  domain_proxy::perform_incoming( std::move(holder), io_id, handler );
}

void repli::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( !this->suspended() && holder.is_notify() )
  {
    read_lock<mutex_type> lk(_mutex);
    for ( auto& r : _targets )
      r.perform_outgoing( holder.clone() , io_id);
  }
  domain_proxy::perform_outgoing( std::move(holder), io_id);
}


}}
