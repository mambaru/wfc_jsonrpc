#include "statistics.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <wfc/jsonrpc/target.hpp>

namespace wfc{ namespace jsonrpc{ 

void statistics::configure()
{
  std::lock_guard<std::mutex> lk(_mutex);
  _enable_write_size = this->options().enable_write_size;
}
void statistics::reconfigure()
{
  std::lock_guard<std::mutex> lk(_mutex);
  _enable_write_size = this->options().enable_write_size;
  _req_meters.clear();
  _ntf_meters.clear();
}

void statistics::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  if ( this->suspended() )
    return domain_proxy::perform_incoming( std::move(holder), io_id, handler );

  size_t size = holder.size();
  meter_ptr meter;
  if (holder.is_request() )
    meter = this->request_meter_(holder.method(), size);
  else if (holder.is_notify() )
    meter = this->notify_meter_(holder.method(), size);
  else
    meter = this->other_meter_(size);

  //meter->inc(0, size - 1);

  bool enable_write_size = this->_enable_write_size;
  domain_proxy::perform_incoming( std::move(holder), io_id, [handler, meter, enable_write_size]( outgoing_holder outholder)
  {
    if ( enable_write_size )
    {
      auto holder = outholder.clone();
      auto d = holder.detach();
      if ( d != nullptr )
        meter->set_write_size( d->size() );
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
