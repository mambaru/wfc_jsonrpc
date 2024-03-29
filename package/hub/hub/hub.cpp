#include "hub.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{
  
namespace detail {
  
struct incoming
{
  typedef wjrpc::data_type::iterator iterator;
  typedef std::pair<iterator, iterator> pair_type;
  char version[4] = "2.0";
  pair_type method;
  pair_type params;
};

struct incoming_json
{
  typedef incoming::pair_type pair_type;
  typedef ::wjson::iterator_pair<pair_type> pair_json;
  JSON_NAME(jsonrpc)
  JSON_NAME(method)
  JSON_NAME(params)

  typedef ::wjson::object<
    incoming, 
    json::member_list<
      json::member<n_jsonrpc, incoming, char[4], &incoming::version >,
      json::member<n_method, incoming, pair_type, &incoming::method, pair_json>,
      json::member<n_params, incoming, pair_type, &incoming::params, pair_json>
    >
  > type;

  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer  serializer;
};

}

void hub::start()
{
  _counter = 0;
}

void hub::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) 
{
  std::lock_guard<mutex_type> lk(_mutex);
  _map[io_id] = itf;
}

void hub::unreg_io(io_id_t io_id) 
{
  std::lock_guard<mutex_type> lk(_mutex);
  _map.erase(io_id);
}

void hub::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
{
  this->perform_incoming_( holder, io_id);
  domain_proxy::perform_incoming( std::move(holder), io_id, handler);
}
  
void hub::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( holder.is_request() || holder.is_notify() )
  {
    outgoing_holder oholder = holder.clone(_counter);
    auto d = oholder.detach();
    incoming_holder iholder( std::move(d) );
    iholder.parse(nullptr);
    this->perform_incoming_( iholder, io_id);
  }
  domain_proxy::perform_outgoing( std::move(holder), io_id);
}

void hub::perform_incoming_(const incoming_holder& holder, io_id_t io_id)
{
  if ( this->suspended() )
    return;
  
  if ( holder.is_request() || holder.is_notify() )
  {
    std::vector<witf> cli;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      cli.reserve( _map.size() );
      for (const auto& m: _map)
      {
        if ( io_id != m.first )
        {
          cli.push_back(m.second);
        }
      }
    }

    if ( !cli.empty() )
    {
      const wjrpc::incoming& incom = holder.get();
      detail::incoming ntf;
      ntf.method = incom.method;
      ntf.params = incom.params;
      data_type d;
      d.reserve( holder.size() );
      detail::incoming_json::serializer()(ntf, std::back_inserter(d));
      for (auto wc: cli)
      {
        if ( auto c = wc.lock() )
        {
          auto dd = std::make_unique<data_type>(d);
          c->perform_io( std::move(dd), this->get_id(), [](data_ptr) noexcept {});
        }
      }
    }
  }
}

}}
