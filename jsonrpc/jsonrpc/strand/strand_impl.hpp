#pragma once

#include "strand_config.hpp"
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <iow/io/strand/mtholder.hpp>
#include <memory>
#include <mutex>

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
  }
  
  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t h) override
  {
    auto handler = make_handler_( std::move(h) );
    
    if ( _incoming->enabled() )
    {
      auto pholder = std::make_shared<incoming_holder>( std::move(holder) );
      auto ptarget = _target;
      _incoming->post([ptarget, pholder, io_id, handler]()
      {
        ptarget->perform_incoming( std::move(*pholder), io_id, handler);
      });
    }
    else if ( handler != nullptr )
    {
      handler(nullptr);
    }
  }
  
  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) override
  {
    if ( _incoming->enabled() )
    {
      auto pholder = std::make_shared<outgoing_holder>( std::move(holder) );
      auto ptarget = _target;
      _incoming->post([ptarget, pholder, io_id]()
      {
        ptarget->perform_outgoing( std::move(*pholder), io_id);
      });
    }
    
  }
  
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
  {
    _target->reg_io(io_id, itf);
  }

  virtual void unreg_io(io_id_t io_id)
  {
    _target->unreg_io(io_id);
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t h)
  {
    std::cout << "make_handler_... " << d << std::endl;
    auto handler = make_handler_( std::move(h) );
    std::cout << "make_handler_... ready" << std::endl;

    if ( _incoming->enabled() )
    {
      auto pd = std::make_shared<data_ptr>( std::move(d) );
      auto ptarget = _target;
      std::cout << "==1==" << std::endl;
      _incoming->post([ptarget, pd, io_id, handler]()
      {
        std::cout << "--1--" << std::endl;
        ptarget->perform_io( std::move(*pd), io_id, handler);
        std::cout << "--2--" << std::endl;
      });
      std::cout << "==2==" << std::endl;
    }
    else if ( handler != nullptr )
    {
      handler(nullptr);
    }
    
  }
  
private:
  
  
  outgoing_handler_t make_handler_(outgoing_handler_t handler)
  {
    std::cout << "make_handler_ 1" << std::endl;
    if ( handler!=nullptr && _outgoing->enabled() )
    {
      std::cout << "make_handler_ 2" << std::endl;
      std::weak_ptr<strand_impl> wthis = this->shared_from_this();
      std::cout << "make_handler_ 3" << std::endl;
      outgoing_handler_t h = [wthis, handler]( data_ptr d)
      {
        std::cout << "make_handler_ call  " << d << std::endl;
        if ( auto pthis = wthis.lock() )
        {
          auto pd = std::make_shared<data_ptr>( std::move(d));
          std::cout << "make_handler_ post ..." << std::endl;
          std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++1> " << pthis->_outgoing->size() << std::endl;
          pthis->_outgoing->post([pd, handler](){
            std::cout << "make_handler_ READY" << std::endl;
            handler(std::move(*pd));
          });
          std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++2>" << pthis->_outgoing->size() << std::endl;
          std::cout << "make_handler_ posted" << std::endl;
        }
      };
      std::cout << "make_handler_ 4" << std::endl;
      auto h1 =  std::move(h);
      std::cout << "make_handler_ 5" << std::endl;
      return std::move(h1);
    }
    return std::move(handler);
  }

private:

  strand_ptr _incoming;
  strand_ptr _outgoing;
  std::shared_ptr<ijsonrpc> _target;
};

}
