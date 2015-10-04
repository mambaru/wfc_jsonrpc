#include "service.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

namespace wfc{

class service::impl
{
public:
  impl( ::wfc::asio::io_service& )
  {}
};

service::~service()
{
}

service::service()
{
}

void service::reconfigure()
{
}

void service::stop(const std::string&) 
{
}

void service::start(const std::string&)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<service::impl>( g->io_service );
    //auto opt = this->options();
  }
}

void service::reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/)
{
  DEBUG_LOG_MESSAGE("service::reg_io")
}

void service::unreg_io(io_id_t )
{
  DEBUG_LOG_MESSAGE("service::unreg_io")
}

void service::perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("service::perform_io io_id=" << io_id << "[" << d << "]")
}

void service::perform_incoming(incoming_holder, io_id_t, rpc_outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("service::perform_incoming")
  abort();
}
  
void service::perform_outgoing(outgoing_holder , io_id_t )
{
  DEBUG_LOG_MESSAGE("service::perform_outgoing")
  abort();
}


}
