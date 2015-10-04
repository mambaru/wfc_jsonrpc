
#include "strand_impl.hpp"
#include "strand.hpp"
#include <wfc/asio.hpp>

namespace wfc{

class strand::impl: public strand_impl
{
public:
  impl( ::wfc::asio::io_service& io)
    : strand_impl(io)
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
#warning Сделать нормальный реконфиг
  if ( auto g = this->global() )
  {
    const auto& conf = this->options();
    auto target = this->global()->registry.get<ijsonrpc>(conf.target);
    if (_impl == nullptr)
    {
      _impl = std::make_shared<strand::impl>( g->io_service );
    }
    _impl->start( conf.incoming, conf.outgoing, target);
  }

}

void strand::stop(const std::string&) 
{
  if ( _impl != nullptr )
  {
    _impl->stop();
  }
}


void strand::start(const std::string&)
{
#warning TODO start
  /*
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<strand::impl>( g->io_service );
    _impl->start();
  }
  */
}


void strand::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  if ( _impl != nullptr )
  {
    _impl->reg_io(io_id, itf);
  }
}

void strand::unreg_io(io_id_t io_id)
{
  if ( _impl != nullptr )
  {
    _impl->unreg_io(io_id);
  }
}

void strand::perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t handler) 
{
  if ( _impl != nullptr )
  {
    _impl->perform_io( std::move(d), io_id, std::move(handler) );
  }
}

void strand::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  if ( _impl != nullptr )
  {
    _impl->perform_incoming( std::move(holder), io_id, std::move(handler) );
  }
}
  
void strand::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( _impl != nullptr )
  {
    _impl->perform_outgoing( std::move(holder), io_id );
  }
}


}
