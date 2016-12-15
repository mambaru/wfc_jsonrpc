#include "statistics.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <wfc/jsonrpc/target.hpp>

namespace wfc{ namespace jsonrpc{ 

void statistics::configure()
{
  std::lock_guard<std::mutex> lk(_mutex);
  _enable_write_size = this->options().enable_write_size;
  _enable_error_stat = this->options().enable_error_stat;
}
void statistics::reconfigure()
{
  std::lock_guard<std::mutex> lk(_mutex);
  _enable_write_size = this->options().enable_write_size;
  _enable_error_stat = this->options().enable_error_stat;
  _req_meters.clear();
  _ntf_meters.clear();
}

void statistics::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  if ( this->suspended() )
    return domain_proxy::perform_incoming( std::move(holder), io_id, handler );

  size_t size = holder.size();
  std::string method =  holder.method();
  meter_ptr meter;
  if (holder.is_request() )
    meter = this->request_meter_(method, size);
  else if (holder.is_notify() )
    meter = this->notify_meter_(method, size);
  else
    meter = this->other_meter_(size);

  //meter->inc(0, size - 1);

  std::weak_ptr< wfc::statistics > wstat;
  bool enable_write_size = this->_enable_write_size;
  bool enable_error_stat = this->_enable_error_stat;
  if ( enable_write_size || enable_error_stat)
    wstat = this->get_statistics();
  domain_proxy::perform_incoming( std::move(holder), io_id, [handler, meter, wstat, enable_write_size, enable_error_stat, method]( outgoing_holder outholder)
  {
    if ( auto stat = wstat.lock() )
    {
      auto holder = outholder.clone();
      if ( auto d = holder.detach() )
      {
        if ( enable_write_size && meter)
          meter->set_write_size( d->size() );
        if ( enable_error_stat )
        {
          wfc::json::json_error e;
          wfc::jsonrpc::outgoing_error<wfc::jsonrpc::error> err;
          typedef wfc::jsonrpc::outgoing_error_json<wfc::jsonrpc::error_json> error_json;
          error_json::serializer()(err, d->begin(), d->end(), &e);
          if ( !e && err.error!=nullptr )
          {
            std::string message;
            wfc::jsonrpc::error_codes_json::serializer()( err.error->code, std::back_inserter(message) );
            if ( message.size() > 2 )
              message = message.substr(1, message.size()-2 );
            else
              message="Error";
            COMMON_LOG_ERROR( "JSON-RPC: " << d )
            auto proto = stat->create_value_prototype("JSON-RPC:" + message);
            stat->create_meter(proto, 0, 1);
            auto mproto = stat->create_value_prototype(method + ":" + message);
            stat->create_meter(mproto, 0, 1);
          }
        }
      }
    }
    handler( std::move(outholder) );    
  } );
}

statistics::meter_ptr statistics::request_meter_(std::string name, size_t size)
{
  auto stat = this->get_statistics();
  if ( stat == nullptr ) return nullptr;

  std::lock_guard<std::mutex> lk(_mutex);
  auto itr = _req_meters.find(name);
  if (itr == _req_meters.end() )
  {
    auto opt = this->options();
    auto prototype = stat->create_composite_prototype(
      !opt.time_suffix.empty()       ?  opt.request_prefix + name + opt.time_suffix : "",
      !opt.read_size_suffix.empty()  ?  opt.request_prefix + name + opt.read_size_suffix : "",
      !opt.write_size_suffix.empty()
      && this->_enable_write_size    ?  opt.request_prefix + name + opt.write_size_suffix : ""
    );
    itr = _req_meters.insert( std::make_pair(name, prototype) ).first;
  }
  return stat->create_meter(itr->second, size);
}

statistics::meter_ptr statistics::notify_meter_(std::string name, size_t size)
{
  auto stat = this->get_statistics();
  if ( stat == nullptr ) return nullptr;

  auto opt = this->options();
  std::lock_guard<std::mutex> lk(_mutex);
  auto itr = _ntf_meters.find(name);
  if (itr == _ntf_meters.end() )
  {
    auto prototype = stat->create_composite_prototype(
      !opt.time_suffix.empty()       ?  opt.notify_prefix + name + opt.time_suffix : "",
      !opt.read_size_suffix.empty()  ?  opt.notify_prefix + name + opt.read_size_suffix : "",
      ""
    );
    itr = _ntf_meters.insert( std::make_pair(name, prototype) ).first;
  }
  return stat->create_meter(itr->second, size);
}

statistics::meter_ptr statistics::other_meter_(size_t size)
{
  auto stat = this->get_statistics();
  if ( stat == nullptr ) return nullptr;

  auto opt = this->options();
  std::lock_guard<std::mutex> lk(_mutex);
  if ( _other == nullptr )
    _other = stat->create_composite_prototype( opt.other_time, opt.other_read_size, this->_enable_write_size ? opt.other_write_size : "" );
  return stat->create_meter( _other, size );
}

}}
