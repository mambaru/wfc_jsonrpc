#include "hub.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{

hub::hub()
{}

void hub::ready()
{
}


void hub::perform_incoming(incoming_holder /*holder*/, io_id_t /*io_id*/, rpc_outgoing_handler_t /*handler*/) 
{
  if ( this->suspended()  ) return;
}
  
void hub::perform_outgoing(outgoing_holder /*holder*/, io_id_t /*io_id*/)
{
  if ( this->suspended() ) return;
}


}}
