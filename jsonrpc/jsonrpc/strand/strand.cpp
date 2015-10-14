
#include "strand_impl.hpp"
#include "strand.hpp"
#include <wfc/asio.hpp>

#include <iow/jsonrpc/engine/call_map.hpp>

namespace wfc{

class jsonrpc_outgoing
  : public ::wfc::ijsonrpc
  , public std::enable_shared_from_this<jsonrpc_outgoing>
{
  typedef std::shared_ptr< ::wfc::iinterface> target_ptr;

public:

  jsonrpc_outgoing( std::shared_ptr< ::wfc::iinterface> target)
    : _target( target )
  {
  }
  
  
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
  {
    _target->reg_io( io_id, itf );
  }

  virtual void unreg_io(io_id_t io_id)
  {
    _target->unreg_io( io_id );
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler)
  {
    _target->perform_io( std::move(d), io_id, std::move(handler) );
  }

  virtual void perform_incoming(incoming_holder /*holder*/, io_id_t /*io_id*/, rpc_outgoing_handler_t /*handler*/) 
  {
    abort();
  }
  
  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) 
  {
    auto rh = holder.result_handler();
    if ( rh == nullptr )
    {
      _target->perform_io( holder.detach(), io_id, nullptr);
    }
    else
    {
      _target->perform_io( holder.detach(), io_id, [rh](data_ptr d)
      {
        std::cout << "starand wrapper " << d << std::endl;
        incoming_holder holder( std::move(d) );
        holder.parse();
        rh( std::move(holder) );
      });
    }
  }

  
private:
  target_ptr _target;
  //call_map _call_map;
  std::atomic<int> _call_counter;
  
};
  
  
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
    
    if (_impl == nullptr)
    {
      _impl = std::make_shared<strand::impl>( g->io_service );
    }

    if ( auto target = this->global()->registry.get<ijsonrpc>(conf.target, true) )
    {
      _impl->start( conf.incoming, conf.outgoing, target);
    }
    else if ( auto target = this->global()->registry.get<iinterface>(conf.target, true) )
    {
      auto jtarget = std::make_shared<jsonrpc_outgoing>( target );
      _impl->start( conf.incoming, conf.outgoing, jtarget);
    }
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
