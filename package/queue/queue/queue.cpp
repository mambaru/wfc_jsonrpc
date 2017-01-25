#include "queue.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{

queue::queue()
{}

void queue::ready()
{
  const auto& opt = this->options();
  if ( opt.callback_queue == true )
    _callback_workflow = this->get_workflow( opt.callback_workflow );
  else
    _callback_workflow = nullptr;
}

void queue::stop()
{
  _callback_workflow = nullptr;
}

void queue::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  if ( this->suspended()  )
  {
    this->get_target().perform_incoming( std::move( holder ), io_id, std::move(handler) );
  }
  else
  {
    auto pholder = std::make_shared<incoming_holder>( std::move(holder) );
    this->get_workflow()->post(
      [pholder, io_id, handler, this]() mutable
      {
        auto t = this->get_target();
        t.perform_incoming( std::move( *pholder ), io_id, this->make_handler_( std::move(handler) ) );
      },
      [pholder, handler]()
      {
        if ( handler != nullptr )
        {
          jsonrpc::outgoing_error< jsonrpc::error > error_message;
          error_message.error = std::make_unique<jsonrpc::error>( jsonrpc::error_codes::QueueOverflow );
          auto id_range = pholder->raw_id();
          if ( id_range.first != id_range.second )
            error_message.id = std::make_unique<data_type>( id_range.first, id_range.second );
          handler( pholder->detach() );
        }
      }
    );
  }
}
  
void queue::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( this->suspended() )
  {
    this->get_target().perform_outgoing( std::move( holder ), io_id);
  }
  else 
  {
    auto pholder = std::make_shared<outgoing_holder>( std::move(holder) );
    this->get_workflow()->post(
      [pholder, io_id, this]()
      {
        auto t = this->get_target();
        t.perform_outgoing( std::move( *pholder ), io_id );
      },
      nullptr
    );
  }
}

queue::rpc_outgoing_handler_t queue::make_handler_(rpc_outgoing_handler_t handler)
{
  if ( _callback_workflow == nullptr ) return std::move(handler);
  
  auto fun = [handler, this](outgoing_holder holder)
  {
    if ( auto w = this->_callback_workflow )
    {
      auto pholder = std::make_shared<outgoing_holder>( std::move(holder) );
      w->post(
        [pholder, handler]()
        {
          handler( std::move( *pholder ) );
        },
        nullptr
      );
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
