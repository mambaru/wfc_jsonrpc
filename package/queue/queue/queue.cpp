#include "queue.hpp"

#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{

queue::queue()
{}

void queue::ready()
{
  const auto& opt = super::options();
  if ( opt.callback_queue == true )
    _callback_workflow = super::get_workflow( opt.callback_workflow );
  else
    _callback_workflow = nullptr;
  _connection_tracking = opt.connection_tracking;
}

void queue::stop()
{
  _callback_workflow = nullptr;
}

void queue::unreg_io(io_id_t io_id) 
{
  if ( _connection_tracking )
  {
    std::lock_guard<mutex_type> lk(_tracking_mutex);
    _tracking_map.erase(io_id);
  }
}

void queue::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
{
  if ( this->suspended()  )
    return super::get_target().perform_incoming( std::move( holder ), io_id, std::move(handler) );

  auto pholder = std::make_shared<incoming_holder>( std::move(holder) );
  super::get_workflow()->post(
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

std::function<void()> queue::make_post_fun_(const std::shared_ptr<incoming_holder> pholder, io_id_t io_id, outgoing_handler_t handler)
{
  auto res_fun = 
    [pholder, io_id, handler, this]()
    { 
      auto t = this->get_target();
      t.perform_incoming( std::move( *pholder ), io_id, this->make_outgoing_handler_( std::move(handler) ) );
    };
  
  if ( !( this->_connection_tracking && pholder->is_request() ) )
    return res_fun;

  return make_track_fun_(io_id, res_fun);
}

std::function<void()> queue::make_track_fun_(io_id_t io_id, std::function<void()> fun)
{
  std::weak_ptr<size_t> wc;
  std::lock_guard<mutex_type> lk(_tracking_mutex);
  auto itr = _tracking_map.find(io_id);
  if ( itr!=_tracking_map.end() )
  {
    ++*(itr->second);
    wc=itr->second;
  }
  else
  {
    wc = _tracking_map.insert( std::make_pair(io_id, std::make_shared<size_t>(1)) ).first->second;
  }
  return [fun, wc, this]()
  {
    if ( wc.lock()!=nullptr )
    {
      fun();
    }
    else
    {
      JSONRPC_LOG_WARNING("JSONRPC-QUEUE " << this->name() << ": request drop from queue by connection tracking")
    }
  };
}

queue::outgoing_handler_t queue::make_outgoing_handler_(outgoing_handler_t handler)
{
  if ( _callback_workflow == nullptr ) return std::move(handler);
  
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
  return std::move(fun);
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
