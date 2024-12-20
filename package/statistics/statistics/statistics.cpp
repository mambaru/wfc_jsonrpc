#include "statistics.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <wfc/jsonrpc.hpp>

namespace wfc{ namespace jsonrpc{

void statistics::configure()
{
  std::lock_guard<mutex_type> lk(_mutex);
  _enable_write_size = this->options().enable_write_size;
  _enable_error_stat = this->options().enable_error_stat;
}

void statistics::reconfigure()
{
  std::lock_guard<mutex_type> lk(_mutex);
  _enable_write_size = this->options().enable_write_size;
  _enable_error_stat = this->options().enable_error_stat;
  _req_meters.clear();
  _ntf_meters.clear();
}

static void static_error_meter(const std::string& method, statistics::data_ptr d,
                               std::shared_ptr< wfc::statistics::statistics > stat)
{
  wfc::json::json_error e;
  wfc::jsonrpc::outgoing_error<wfc::jsonrpc::error> err;
  typedef wfc::jsonrpc::outgoing_error_json<wfc::jsonrpc::error_json> error_json;
  error_json::serializer()(err, d->begin(), d->end(), &e);
  if ( !e && err.error!=nullptr )
  {
    /*
    std::string message;
    wfc::jsonrpc::error_codes_json::serializer()( err.error->code, std::back_inserter(message) );
    if ( message.size() > 2 )
      message = message.substr(1, message.size()-2 );
    else
      message="Unknown Error";*/
    // Убрать сообщение в иени, смотрим в лог
    auto mproto = stat->create_value_meter(method /* + ":" + message*/);
    mproto.create( static_cast<wrtstat::value_type>(0), static_cast<wrtstat::size_type>(1) );
  }
}

void statistics::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler)
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

  std::weak_ptr< wfc::statistics::statistics > wstat;
  bool enable_write_size = this->_enable_write_size;
  bool enable_error_stat = this->_enable_error_stat;
  if ( enable_write_size || enable_error_stat)
  {
    wstat = this->get_statistics();
    auto err_method = this->name() + this->statistics_options().error_prefix + method;
    domain_proxy::perform_incoming( std::move(holder), io_id,
      [handler, meter, wstat, enable_write_size, enable_error_stat, err_method]( outgoing_holder outholder)
      {
        if ( auto stat = wstat.lock() )
        {
          if ( outholder.is_result() )
          {
            auto call_id = outholder.call_id();
            // call_id передается по ссылке для инкремента id запросов
            // для result без изменений
            auto oholder = outholder.clone(call_id);
            if ( auto d = oholder.detach() )
            {
              if ( enable_write_size )
                meter->set_write_size( static_cast<wrtstat::value_type>( d->size() ) );
              if ( enable_error_stat )
                static_error_meter( err_method, std::move(d), stat);
            }
          }
          // else: TODO: статистику для встречных вызовов
        }
        handler( std::move(outholder) );
      }
    );
  }
  else
  {
    domain_proxy::perform_incoming( std::move(holder), io_id, std::move(handler) );
  }

}

void statistics::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( this->suspended() )
    return domain_proxy::perform_outgoing( std::move(holder), io_id);
  // TODO: для исходящих стата тоже может быть нужна
  domain_proxy::perform_outgoing( std::move(holder), io_id);
}

statistics::meter_ptr statistics::request_meter_(std::string meter_name, size_t size)
{
  auto stat = this->get_statistics();
  if ( stat == nullptr ) return nullptr;

  std::lock_guard<mutex_type> lk(_mutex);
  auto itr = _req_meters.find(meter_name);
  if (itr == _req_meters.end() )
  {
    auto opt = this->statistics_options();
    auto prototype = stat->create_composite_meter(
      !opt.time_suffix.empty()       ?  this->name() + opt.request_prefix + meter_name + opt.time_suffix : "",
      !opt.read_size_suffix.empty()  ?  this->name() + opt.request_prefix + meter_name + opt.read_size_suffix : "",
      !opt.write_size_suffix.empty()
      && this->_enable_write_size    ?  this->name() + opt.request_prefix + meter_name + opt.write_size_suffix : "",
      true
    );
    itr = _req_meters.insert( std::make_pair(meter_name, prototype) ).first;
  }
  return itr->second.create_shared(
    static_cast<wrtstat::size_type>(1),
    static_cast<wrtstat::value_type>(size),
    static_cast<wrtstat::value_type>(0)
  );
}

statistics::meter_ptr statistics::notify_meter_(std::string meter_name, size_t size)
{
  auto stat = this->get_statistics();
  if ( stat == nullptr ) return nullptr;

  auto opt = this->statistics_options();
  std::lock_guard<mutex_type> lk(_mutex);
  auto itr = _ntf_meters.find(meter_name);
  if (itr == _ntf_meters.end() )
  {
    auto prototype = stat->create_composite_meter(
      !opt.time_suffix.empty()       ?  this->name() + opt.notify_prefix + meter_name + opt.time_suffix : "",
      !opt.read_size_suffix.empty()  ?  this->name() + opt.notify_prefix + meter_name + opt.read_size_suffix : "",
      "",
      true
    );
    itr = _ntf_meters.insert( std::make_pair(meter_name, prototype) ).first;
  }
  return itr->second.create_shared(
    static_cast<wrtstat::size_type>(1),
    static_cast<wrtstat::value_type>(size),
    static_cast<wrtstat::value_type>(0)
  );

}

statistics::meter_ptr statistics::other_meter_(size_t size)
{
  auto stat = this->get_statistics();
  if ( stat == nullptr ) return nullptr;

  auto opt = this->statistics_options();
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _other.size() == 0 )
  {
    _other = stat->create_composite_meter(
      this->name() + opt.other_time,
      this->name() + opt.other_read_size,
      this->_enable_write_size ? this->name() + opt.other_write_size : "",
      true
    );
  }
  return _other.create_shared(
    static_cast<wrtstat::size_type>(1),
    static_cast<wrtstat::value_type>(size),
    static_cast<wrtstat::value_type>(0)
  );
}

}}
