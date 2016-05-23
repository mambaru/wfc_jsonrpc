#pragma once

#include "strand_config.hpp"
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <iow/io/strand/mtholder.hpp>
#include <memory>
#include <mutex>
#include <boost/concept_check.hpp>

namespace wfc{ 

class strand_impl
  : public ijsonrpc
  , public std::enable_shared_from_this<strand_impl>
{
  typedef ::iow::io::strand::mtholder<> strand_type;
  typedef std::shared_ptr<strand_type> strand_ptr;
  typedef std::recursive_mutex mutex_type;
public:
  
  typedef strand_type::io_service_type io_service_type;
  
  strand_impl(io_service_type& io)
  {
    _incoming = std::make_shared<strand_type>(io);
    _outgoing = std::make_shared<strand_type>(io);
  };
  
  void start(
    strand_config::strand_options opt1,
    strand_config::strand_options opt2,
    std::shared_ptr<ijsonrpc> target
  )
  {
    _incoming->start(opt1);
    _outgoing->start(opt2);
    _target = target;
  }
  
  void stop()
  {
    _incoming->stop();
    _outgoing->stop();
    _target = nullptr;
  }
  
  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t h) override
  {
    auto handler = make_handler_( std::move(h) );
    auto pholder = std::make_shared<incoming_holder>( std::move(holder) );
    auto ptarget = _target;
    _incoming->post([ptarget, pholder, io_id, handler]()
    {
      ptarget->perform_incoming( std::move(*pholder), io_id, handler);
    });
  }

  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) override
  {
    this->make_result_(holder);
    auto pholder = std::make_shared<outgoing_holder>( std::move(holder) );
    auto ptarget = _target;
    _outgoing->post([ptarget, pholder, io_id]()
    {
      ptarget->perform_outgoing( std::move(*pholder), io_id);
    });
  }

  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
  {
    _target->reg_io(io_id, itf);
  }

  virtual void unreg_io(io_id_t io_id)
  {
    _target->unreg_io(io_id);
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t handler)
  {
    auto pd = std::make_shared<data_ptr>( std::move(d) );
    auto ptarget = _target;
    _incoming->post([ptarget, pd, io_id, handler]()
    {
      ptarget->perform_io( std::move(*pd), io_id, handler);
    });
  }

private:

  rpc_outgoing_handler_t make_handler_(rpc_outgoing_handler_t handler)
  {
    if ( handler!=nullptr && _outgoing->enabled() )
    {
      std::weak_ptr<strand_impl> wthis = this->shared_from_this();
      rpc_outgoing_handler_t h = [wthis, handler]( outgoing_holder holder)
      {
        if ( auto pthis = wthis.lock() )
        {
          auto pd = std::make_shared<outgoing_holder>( std::move(holder));
          pthis->_outgoing->post([pd, handler]()
          {
            handler(std::move(*pd));
          });
        }
      };
      auto h1 =  std::move(h);
      return std::move(h1);
    }
    return std::move(handler);
  }
  
  void make_result_(outgoing_holder& holder)
  {
    if ( !_incoming->enabled() ) 
      return;
    
    if ( auto orig = holder.result_handler() )
    {
      std::weak_ptr<strand_impl> wthis = this->shared_from_this();
      outgoing_holder::result_handler_t h=[wthis, orig](incoming_holder holder)
      {
        if ( auto pthis = wthis.lock() )
        {
          auto pholder = std::make_shared<incoming_holder>( std::move(holder) );
          pthis->_incoming->post([pholder, orig]()
          {
            orig(std::move(*pholder));
          });
        }
      };

      holder.result_handler( std::move(h) );
    }
  }

private:

  strand_ptr _incoming;
  strand_ptr _outgoing;
  std::shared_ptr<ijsonrpc> _target;
};

}
