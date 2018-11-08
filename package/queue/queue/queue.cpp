#include "queue.hpp"

#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{

queue::queue()
{}

void queue::restart()
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

void queue::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
{
  if ( this->suspended()  )
    return this->get_target().perform_incoming( std::move( holder ), io_id, std::move(handler) );

  auto pholder = std::make_shared<incoming_holder>( std::move(holder) );
  this->get_workflow()->post(
    this->make_post_fun_(pholder, io_id, handler),
    std::bind(&queue::drop_handler_, this, pholder, handler )
  );
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

std::function<void()> queue::make_post_fun_(const std::shared_ptr<incoming_holder>& pholder, io_id_t io_id, outgoing_handler_t handler)
{
  auto res_fun = 
    [pholder, io_id, handler, this]()
    { 
      auto t = this->get_target();
      t.perform_incoming( std::move( *pholder ), io_id, this->make_outgoing_handler_( std::move(handler) ) );
    };
  
  if ( pholder->is_request() )
    return make_track_fun_(io_id, std::move(res_fun) );
  
  return res_fun;
}

std::function<void()> queue::make_track_fun_(io_id_t io_id, std::function<void()> fun)
{
  std::weak_ptr<queue> wthis = this->shared_from_this();;
  return this->tracking(io_id, std::move(fun), [wthis]()
    {
      std::string tname;
      if ( auto pthis = wthis.lock() )
        tname = pthis->name();
      JSONRPC_LOG_WARNING("Tracking jsonrpc-queue '" << tname << "': request drop by tracking.")
    }
  );
}

queue::outgoing_handler_t queue::make_outgoing_handler_(outgoing_handler_t handler)
{
  if ( _callback_workflow == nullptr ) 
    return handler;
  
  outgoing_handler_t fun = [handler, this](outgoing_holder holder)
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
  return fun;
}

void queue::drop_handler_(const std::shared_ptr<incoming_holder> pholder, outgoing_handler_t handler)
{
  if ( handler == nullptr )
    return;
  
  jsonrpc::outgoing_error< jsonrpc::error > error_message;
  error_message.error = std::make_unique<jsonrpc::error>( jsonrpc::error_codes::QueueOverflow );
  auto id_range = pholder->raw_id();
  if ( id_range.first != id_range.second )
    error_message.id = std::make_unique<data_type>( id_range.first, id_range.second );
  auto d = pholder->detach();
  if ( d == nullptr )
    d = std::make_unique<data_type>();
  d->clear();
  d->reserve(80);
  outgoing_error_json<error_json >::serializer()(error_message, std::inserter( *d, d->end() ));
  handler( outgoing_holder(std::move(d)) );
}

}}
