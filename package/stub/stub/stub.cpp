#include "stub.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <wfc/jsonrpc/consumer_adapter.hpp>

#include <wjrpc/outgoing/outgoing_result_json.hpp>
#include <wjrpc/outgoing/outgoing_error_json.hpp>

#include <wflow/owner.hpp>

namespace wfc{ namespace jsonrpc{

class stub::stab_handler
  : public consumer_adapter
{
  typedef std::mutex mutex_type;

public:
  explicit stab_handler(consumer_adapter::itf_ptr_t itf, const stub_config& opt)
    : consumer_adapter(itf)
    , _opt(opt)
  {
  }

  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) override
  {
    auto raw_id = holder.raw_id();
    if ( _opt.mode == stub_config::advance_response && (holder.is_response() || holder.is_error()) )
    {
      std::string str_id(raw_id.first, raw_id.second);
      std::lock_guard<mutex_type> lk(_mutex);
      auto itr = _wait_responce.find(str_id);
      if ( itr!=_wait_responce.end() )
      {
        _wait_responce.erase(str_id);
        return;
      }
    }

    target_adapter::perform_incoming(
      this->make( std::move(holder), handler),
      io_id,
      this->make( std::move(handler), raw_id)
    );
  }

  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) override
  {
    target_adapter::perform_outgoing( this->make(std::move(holder)), io_id);
  }

  template<typename I>
  data_ptr make_result_(I beg, I end)
  {
    wjrpc::outgoing_result<std::string> result;
    result.id = std::make_unique<data_type>(beg, end);
    if ( _opt.fake_result!=nullptr )
      result.result = std::make_unique<std::string>(*_opt.fake_result);
    data_ptr d = std::make_unique<data_type>();
    wjrpc::outgoing_result_json< wjson::raw_value<> >::serializer()(result, std::back_inserter(*d));
    return d;
  }

  data_ptr make_result_(incoming_holder::raw_t id)
  {
    return make_result_(id.first, id.second);
  }

  data_ptr make_result_(wjrpc::call_id_t id)
  {
    auto strid = std::to_string(id);
    return make_result_(strid.begin(), strid.end());
  }

  stab_handler::outgoing_holder make(outgoing_holder holder)
  {
    if ( _opt.to_notify && holder.is_request() )
    {
      if ( _opt.mode != stub_config::advance_response )
        holder.result_handler()( incoming_holder( make_result_(holder.call_id() ) ) );
      holder = holder.clone_notify();

    }
    else if ( _opt.to_request && holder.is_notify() )
    {
      holder = holder.clone_request(_counter);
      holder.result_handler([](incoming_holder)noexcept{});
    }
    return holder;
  }

  stab_handler::incoming_holder make(incoming_holder holder, const outgoing_handler_t& handler)
  {
    if (holder.ready())
      holder.parse(nullptr);

    if ( _opt.to_notify && holder.is_request() )
    {
      if ( _opt.mode !=stub_config::advance_response )
        handler( outgoing_holder(make_result_(holder.raw_id())));
      holder = holder.clone_notify();
    }
    else if ( _opt.to_request && holder.is_notify() )
    {
      holder = holder.clone_request(_counter);
    }
    else if ( (_opt.mode == stub_config::ignore_errors || _opt.mode == stub_config::fake_response) && holder.is_error())
    {
      holder = incoming_holder( make_result_(holder.raw_id()), holder.get_time_point() );
    }
    else if ( _opt.mode == stub_config::fake_response && holder.is_response() )
    {
      holder = incoming_holder( make_result_(holder.raw_id()), holder.get_time_point() );
    }
    return holder;
  }

  outgoing_handler_t make(outgoing_handler_t handler, const incoming_holder::raw_t& raw_id)
  {
    if ( _opt.mode == stub_config::advance_response )
    {
      if ( raw_id.first!=raw_id.second )
      {
        {
          std::lock_guard<mutex_type> lk(_mutex);
          _wait_responce.insert( std::string(raw_id.first,raw_id.second) );
        }
        handler(outgoing_holder(make_result_(raw_id)));
      }
      return [](outgoing_holder) noexcept{};
    }
    else
    {
      return _owner.callback([handler, this](outgoing_holder holder)
      {
        handler(this->make(std::move(holder)));
      });
    }
  }

private:
  stub_config _opt;
  static std::atomic_int _counter;
  wflow::owner _owner;
  std::set<std::string> _wait_responce;
  mutex_type _mutex;
};

std::atomic_int stub::stab_handler::_counter = 0;

stub::stub()
  : _counter(0)
{

}
void stub::reg_io( io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  if (  this->suspended() )
    super::reg_io(io_id, itf);
  else
  {
    auto handler  = std::make_shared<stab_handler>(itf, _conf);
    super::reg_io(io_id, handler);
    std::lock_guard<mutex_type> lk(_mutex);
    _handler_map[io_id] = handler;
  }
}

void stub::unreg_io(io_id_t io_id)
{
  super::unreg_io(io_id);
  std::lock_guard<mutex_type> lk(_mutex);
  _handler_map.erase(io_id);
}

void stub::start()
{
  _counter = 0;
  this->restart();
}

void stub::restart()
{
  _conf=this->options();
}

void stub::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler)
{
  std::shared_ptr<stab_handler> pstub;
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _handler_map.find(io_id);
    if ( itr != _handler_map.end() )
      pstub = itr->second;
    else
    {
      DOMAIN_LOG_WARNING("stub::perform_incoming: requried call stub::reg_io")
    }
  }

  if ( this->suspended() || pstub==nullptr)
    return domain_proxy::perform_incoming( std::move(holder), io_id, handler );

  auto new_handler = pstub->make(handler, holder.raw_id());
  domain_proxy::perform_incoming(
    pstub->make( std::move(holder) , handler),
    io_id,
    new_handler
  );
}

void stub::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( !this->suspended()  )
  {
    std::shared_ptr<stab_handler> pstub;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      auto itr = _handler_map.find(io_id);
      if ( itr != _handler_map.end() )
        pstub = itr->second;
      else
      {
        DOMAIN_LOG_WARNING("stub::perform_incoming: requried call stub::reg_io")
      }
    }

    if ( pstub!=nullptr)
      holder = pstub->make( std::move(holder));
  }
  domain_proxy::perform_outgoing( std::move(holder), io_id);
}

}}
