#include "broker.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

namespace wfc{

class broker::impl
{
public:
  impl( ::wfc::asio::io_service& )
  {}
};

broker::~broker()
{
}

broker::broker()
{
}

void broker::reconfigure()
{
}

void broker::stop(const std::string&) 
{
}

void broker::start(const std::string&)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<broker::impl>( g->io_service );
    //auto opt = this->options();
  }
}

void broker::reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/)
{
  DEBUG_LOG_MESSAGE("broker::reg_io")
}

void broker::unreg_io(io_id_t )
{
  DEBUG_LOG_MESSAGE("broker::unreg_io")
}

void broker::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("broker::perform_io io_id=" << io_id << "[" << d << "]")
}

void broker::perform_incoming(incoming_holder, io_id_t, outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("broker::perform_incoming")
  abort();
}
  
void broker::perform_outgoing(outgoing_holder , io_id_t )
{
  DEBUG_LOG_MESSAGE("broker::perform_outgoing")
  abort();
}


}
