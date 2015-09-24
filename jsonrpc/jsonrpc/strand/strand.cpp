#include "strand.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

namespace wfc{

class strand::impl
{
public:
  impl( ::wfc::asio::io_service& )
  {}
};

strand::~strand()
{
}

strand::strand()
{
}

void strand::reconfigure()
{
}

void strand::stop(const std::string&) 
{
}

void strand::start(const std::string&)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<strand::impl>( g->io_service );
    //auto opt = this->options();
  }
}

void strand::reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/)
{
  DEBUG_LOG_MESSAGE("strand::reg_io")
}

void strand::unreg_io(io_id_t )
{
  DEBUG_LOG_MESSAGE("strand::unreg_io")
}

void strand::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("strand::perform_io io_id=" << io_id << "[" << d << "]")
}

void strand::perform_incoming(incoming_holder, io_id_t, outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("strand::perform_incoming")
  abort();
}
  
void strand::perform_outgoing(outgoing_holder , io_id_t )
{
  DEBUG_LOG_MESSAGE("strand::perform_outgoing")
  abort();
}


}
