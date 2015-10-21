#include "repli.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

namespace wfc{ namespace jsonrpc{ namespace repli{

class repli::impl
{
public:
  impl( ::wfc::asio::io_service& )
  {}
};

repli::~repli()
{
}

repli::repli()
{
}

void repli::reconfigure()
{
}

void repli::stop(const std::string&) 
{
}

void repli::start(const std::string&)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<repli::impl>( g->io_service );
    //auto opt = this->options();
  }
}

void repli::reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/)
{
  DEBUG_LOG_MESSAGE("repli::reg_io")
}

void repli::unreg_io(io_id_t )
{
  DEBUG_LOG_MESSAGE("repli::unreg_io")
}

void repli::perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t ) 
{
  wfc::only_for_log(d, io_id);
  DEBUG_LOG_MESSAGE("repli::perform_io io_id=" << io_id << "[" << d << "]")
}

void repli::perform_incoming(incoming_holder, io_id_t, rpc_outgoing_handler_t ) 
{
  DEBUG_LOG_MESSAGE("repli::perform_incoming")
  abort();
}
  
void repli::perform_outgoing(outgoing_holder , io_id_t )
{
  DEBUG_LOG_MESSAGE("repli::perform_outgoing")
  abort();
}


}}}
