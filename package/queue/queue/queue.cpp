#include "queue.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{

queue::queue()
{}

void queue::reconfigure()
{
  domain_proxy::reconfigure();
  const auto& opt = this->options();
  if ( !opt.incoming_disabled )
  {
    if ( opt.incoming_workflow.empty() )
    {
      _incoming = this->get_workflow();
    }
    else
    {
      _incoming = this->get_target< ::wfc::workflow >( opt.incoming_workflow );
    }
    /*
    _incoming = ::wfc::workflow::recreate_and_start( _incoming, opt.incoming );
    */
  }
  else
  {
    //if ( _incoming!=nullptr ) _incoming->stop();
    _incoming = nullptr;
  }
  
  if ( !opt.outgoing_disabled )
  {
    if ( opt.outgoing_workflow.empty() )
    {
      _outgoing = this->get_workflow();
    }
    else
    {
      _outgoing = this->get_target< ::wfc::workflow >( opt.outgoing_workflow );
    }

    //_outgoing = ::wfc::workflow::recreate_and_start( _outgoing, opt.outgoing );
  }
  else 
  {
    //if ( _outgoing!=nullptr ) _outgoing->stop();
    _outgoing = nullptr;
  }
  
}

void queue::stop(const std::string&)
{
  if ( auto w = _incoming ) w->stop();
  if ( auto w = _outgoing ) w->stop();
  _incoming = nullptr;
  _outgoing = nullptr;
}

void queue::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  if ( this->suspended()  )
  {
    this->target().perform_incoming( std::move( holder ), io_id, std::move(handler) );
  }
  else if ( auto w = _incoming )
  {
    auto pholder = std::make_shared<incoming_holder>( std::move(holder) );
    w->post([pholder, io_id, handler, this]() mutable
    {
      auto t = this->target();
      t.perform_incoming( std::move( *pholder ), io_id, this->make_handler_( std::move(handler) ) );
    });
  }
  else
  {
    auto t = this->target();
    t.perform_incoming( std::move( holder ), io_id, this->make_handler_( std::move(handler) ) );
  }
}
  
void queue::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( this->suspended() )
  {
    this->target().perform_outgoing( std::move( holder ), io_id);
  }
  else if ( auto w = _incoming )
  {
    auto pholder = std::make_shared<outgoing_holder>( std::move(holder) );
    w->post([pholder, io_id, this]()
    {
      auto t = this->target();
      t.perform_outgoing( std::move( *pholder ), io_id );
    });
  }
  else
  {
    auto t = this->target();
    t.perform_outgoing( std::move( holder ), io_id );
  }
}

queue::rpc_outgoing_handler_t queue::make_handler_(rpc_outgoing_handler_t&& handler)
{
  if ( _outgoing == nullptr ) return std::move(handler);
  
  auto fun = [handler, this](outgoing_holder holder)
  {
    if ( auto w = this->_outgoing )
    {
      auto pholder = std::make_shared<outgoing_holder>( std::move(holder) );
      w->post([pholder, handler]()
      {
        handler( std::move( *pholder ) );
      });
    }
    else
    {
      // Если перекофигурировали 
      handler( std::move( holder ) );
    }
  };
  return std::move(fun);
}


}}
