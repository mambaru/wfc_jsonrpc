#include "statistics.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <wfc/jsonrpc/target.hpp>

namespace wfc{ namespace jsonrpc{ 

void statistics::reconfigure()
{
  std::lock_guard<std::mutex> lk(_mutex);
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

  domain_proxy::perform_incoming( std::move(holder), io_id, [handler, meter]( outgoing_holder outholder)
  {
    handler( std::move(outholder) );
  } );
}


statistics::meter_ptr statistics::request_meter_(std::string name, size_t size)
{
  std::lock_guard<std::mutex> lk(_mutex);
  auto itr = _req_meters.find(name);
  if (itr == _req_meters.end() )
  {
    auto prototype = this->create_meter_prototype( std::string("req:") + name + ".rate", std::string("req:") + name + ".size");
    itr = _req_meters.insert( std::make_pair(name, prototype) ).first;
  }
  return this->create_meter(itr->second, size);
}

statistics::meter_ptr statistics::notify_meter_(std::string name, size_t size)
{
  std::lock_guard<std::mutex> lk(_mutex);
  auto itr = _ntf_meters.find(name);
  if (itr == _ntf_meters.end() )
  {
    auto prototype = this->create_meter_prototype( std::string("ntf:") + name + ".rate", std::string("ntf:") + name + ".size");
    itr = _ntf_meters.insert( std::make_pair(name, prototype) ).first;
  }
  return this->create_meter(itr->second, size);
 
}

statistics::meter_ptr statistics::other_meter_(size_t size)
{
  std::lock_guard<std::mutex> lk(_mutex);
  if ( _other == nullptr )
    _other = this->create_meter_prototype("other.rate", "other.size");
  return this->create_meter(_other, size );
}

}}
