#include "gateway.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

namespace wfc{

class gateway::impl
{
public:
  impl( ::wfc::asio::io_service& )
  {}
};

gateway::~gateway()
{
}

gateway::gateway()
{
}

void gateway::reconfigure()
{
}

void gateway::stop(const std::string&) 
{
}

void gateway::start(const std::string&)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<gateway::impl>( g->io_service );
    //auto opt = this->options();
  }
}

void gateway::reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/)
{
  DEBUG_LOG_MESSAGE("gateway::reg_io")
}

void gateway::unreg_io(io_id_t )
{
  DEBUG_LOG_MESSAGE("gateway::unreg_io")
}

void gateway::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("gateway::perform_io io_id=" << io_id << "[" << d << "]")
}

void gateway::perform_incoming(incoming_holder, io_id_t, outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("gateway::perform_incoming")
  abort();
}
  
void gateway::perform_outgoing(outgoing_holder , io_id_t )
{
  DEBUG_LOG_MESSAGE("gateway::perform_outgoing")
  abort();
}


}
