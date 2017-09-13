#include "hub.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{

void hub::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
{
  domain_proxy::perform_incoming( std::move(holder), io_id, handler);
}
  
void hub::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  domain_proxy::perform_outgoing( std::move(holder), io_id);
}


}}
