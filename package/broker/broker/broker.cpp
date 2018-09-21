#include "broker.hpp"
#include <wjrpc/incoming/send_error.hpp>
#include <wfc/logger.hpp>


namespace wfc{ namespace jsonrpc{ 


broker::domain_config broker::generate(const std::string& val)
{
  domain_config conf = super::generate(val);
  if ( !val.empty() )
  {
    domain_config::rule r;
    r.target = std::make_shared<std::string>("<<target-name>>");
    r.methods.insert("<<method-name>>");
    conf.reject.push_back("<<method-name>>");
    conf.rules.push_back(r);
  }
  return conf;
}

void broker::ready()
{
  std::lock_guard<mutex_type> lk(_mutex);
  
  const auto& opt = this->options();
  _reject.clear();
  _reject.insert( opt.reject.begin(), opt.reject.end() );
  _targets.clear();
  std::set<std::string> names;
  for (const auto& r: opt.rules)
  {
    if ( r.target!=nullptr && !r.target->empty() )
      names.insert(*r.target);
  }
  
  for (const auto& name: names)
  {
    auto target = this->get_adapter(name);
    _targets.push_back( target );
  }
  
  for (const auto& r: opt.rules)
  {
    _rules.push_back(rule_target());
    _rules.back().methods = r.methods;
    if ( r.target!=nullptr && !r.target->empty() )
    {
      _rules.back().target = std::make_shared<target_adapter>( this->get_adapter(*r.target) );
    }
    
    if ( r.params!=nullptr && !r.params->empty() )
    {
      _rules.back().matcher = std::make_shared<matchmaker>();
      json::json_error err;
      _rules.back().matcher->reconfigure(r.mode, *r.params, err);
    }
  }
  //_rules.assign(opt.rules.begin(), opt.rules.end());
}

void broker::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) 
{
  domain_proxy::reg_io(io_id, itf);
  read_lock<mutex_type> lk(_mutex);
  for (auto& t : _targets )
  {
    t.reg_io(io_id, itf);
  }
}

void broker::unreg_io(io_id_t io_id) 
{
  domain_proxy::unreg_io(io_id);
  
  read_lock<mutex_type> lk(_mutex);
  for (auto& t : _targets )
  {
    t.unreg_io(io_id);
  }
}


void broker::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
{
  if ( this->suspended() )
  {
    domain_proxy::perform_incoming(std::move(holder), io_id, std::move(handler) );
    return;
  }
  
  if ( !holder.has_method() )
  {
    domain_proxy::perform_incoming(std::move(holder), io_id, std::move(handler) );
    return;
  }
  
  read_lock<mutex_type> lk(_mutex);
  if ( _reject.find( holder.method() ) != _reject.end() )
  {
    this->send_error<service_unavailable>(std::move(holder), std::move(handler));
    return;
  }
  
  for (const auto& r: _rules)
  {
    if ( 0 != r.methods.count(holder.method()) )
    {
      bool match = true;
      if ( r.matcher!=nullptr)
      {
        json::json_error err;
        const char *beg = &(*holder.get().params.first);
        const char *end = &(*holder.get().params.second);
        match = r.matcher->match(beg, end, err);
        if ( err )
        {
          match = false;
          JSONRPC_LOG_ERROR("broker match error: " << json::strerror::message_trace(err, beg, end) );
        }
      }
      if (match)
      {
        if ( r.target!=nullptr )
          r.target->perform_incoming(std::move(holder), io_id, std::move(handler) );
        else
          this->send_error<procedure_not_found>(std::move(holder), std::move(handler));

        return;
      }
    }
  }
    
  if ( this->get_target() ) 
  {
    domain_proxy::perform_incoming(std::move(holder), io_id, std::move(handler) );
    return;
  }

  this->send_error<procedure_not_found>( std::move(holder), std::move(handler) );
}
  
void broker::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( this->suspended() )
  {
    domain_proxy::perform_outgoing( std::move(holder), io_id );
    return;
  }

  if ( !holder.is_request() )
  {
    domain_proxy::perform_outgoing(std::move(holder), io_id );
    return;
  }
    
  read_lock<mutex_type> lk(_mutex);
  if ( _reject.find( holder.name() ) != _reject.end() )
  {
    if ( auto rh = holder.result_handler() )
      rh( incoming_holder( data_ptr() ) );
    return;
  }
    
    /*
  auto itr = _methods.find(holder.name());
  if ( itr != _methods.end() )
  {
    itr->second.perform_outgoing(std::move(holder), io_id );
    return;
  }*/
    
  domain_proxy::perform_outgoing(std::move(holder), io_id );
}


}}
