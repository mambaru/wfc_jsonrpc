#include "hub.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{

void hub::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  //if ( this->suspended()  ) return;
  domain_proxy::perform_incoming( std::move(holder), io_id, handler);
}
  
void hub::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  //if ( this->suspended() ) return;
  domain_proxy::perform_outgoing( std::move(holder), io_id);
}


}}
