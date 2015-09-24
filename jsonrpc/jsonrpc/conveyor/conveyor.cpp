#include "conveyor.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

namespace wfc{

class conveyor::impl
{
public:
  impl( ::wfc::asio::io_service& )
  {}
};

conveyor::~conveyor()
{
}

conveyor::conveyor()
{
}

void conveyor::reconfigure()
{
}

void conveyor::stop(const std::string&) 
{
}

void conveyor::start(const std::string&)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<conveyor::impl>( g->io_service );
    //auto opt = this->options();
  }
}

void conveyor::reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/)
{
  DEBUG_LOG_MESSAGE("conveyor::reg_io")
}

void conveyor::unreg_io(io_id_t )
{
  DEBUG_LOG_MESSAGE("conveyor::unreg_io")
}

void conveyor::perform_io(data_ptr , io_id_t , outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("conveyor::perform_io io_id=" << io_id << "[" << d << "]")
}

void conveyor::perform_incoming(incoming_holder, io_id_t, outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("conveyor::perform_incoming")
  abort();
}
  
void conveyor::perform_outgoing(outgoing_holder , io_id_t )
{
  DEBUG_LOG_MESSAGE("conveyor::perform_outgoing")
  abort();
}


}
