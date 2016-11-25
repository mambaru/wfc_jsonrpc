#include "repli.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <wfc/jsonrpc/target.hpp>

namespace wfc{ namespace jsonrpc{ 

void repli::ready()
{
  
  _targets.clear();
  for ( auto name : this->options().reply_targets )
  {
    _targets.push_back(  this->get_adapter(name) );
  }
}

void repli::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  for ( auto& r : _targets )
    r.perform_incoming( holder.clone(), io_id, handler);
  domain_proxy::perform_incoming( std::move(holder), io_id, handler );
}

void repli::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  for ( auto& r : _targets )
    r.perform_outgoing( holder.clone() , io_id);
  domain_proxy::perform_outgoing( std::move(holder), io_id);
}


}}
