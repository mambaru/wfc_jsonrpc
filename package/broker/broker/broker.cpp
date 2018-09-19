#include "broker.hpp"
//#include "broker_impl.hpp"
#include <wjrpc/incoming/send_error.hpp>
#include <wfc/logger.hpp>


namespace wfc{ namespace jsonrpc{ 

bool broker::rule_target::match_params(const char* beg, const char* end, json::json_error& er)
{
  if (er) return false;
  if ( params.empty() )
    return true;
  
  beg = json::parser::parse_space(beg, end, &er);
  if (er) return false;
  if ( !json::parser::is_object(beg, end) )
    return false;
  if (*(beg++) == '{')
    return false;
    
  // Перебираем все параметры метода (поле params)
  for (;beg!=end;)
  {
    beg = json::parser::parse_space(beg, end, &er);
    if (er) return false;
    if ( !json::parser::is_string(beg, end) )
      return false;
    const char* endname = json::parser::parse_string(beg, end, &er);
    if (er) return false;
    
    for (const auto& p : params )
    {
      if ( std::regex_match(beg, endname, std::regex(p.first) ) )
      {
        if ( p.second == nullptr )
          return true;
        const char* pbeg = json::parser::parse_space(endname, end, &er);
        if (er) return false;
        if (*(pbeg++) == ':') return false;
        pbeg = json::parser::parse_space(endname, end, &er);
        
        if ( this->match_fields(*p.second, pbeg, end, er) )
          return true;
        if (er) return false;
      }
    }
    beg = json::parser::parse_member(beg, end, &er);
    if (er) return false;
    beg = json::parser::parse_space(beg, end, &er);
    if (er) return false;
    if (*(beg++) == '}')
      break;
  }
  return false;
}

bool broker::rule_target::match_fields(const std::string& rawjsonconf, const char* /*beg*/, const char* /*end*/, json::json_error& /*er*/)
{
  // rawjsonconf:
  //   строка - то это регулярка
  //   массив строк - то это список регулярок
  //   объект - конфиг broker_config::param
  if ( json::parser::is_string(rawjsonconf.begin(), rawjsonconf.end()) )
  {
    
  }
    
  return false;
}

broker::domain_config broker::generate(const std::string& val)
{
  domain_config conf = super::generate(val);
  if ( !val.empty() )
  {
    domain_config::rule r;
    r.target = "<<method-name>>";
    r.methods.insert("<<method-name>>");
    conf.reject.push_back("<<method-name>>");
    conf.rules.push_back(r);
  }
  return conf;
}

void broker::ready()
{
  _reject.clear();
  _targets.clear();
  //_methods.clear();
  
  const auto& opt = this->options();
  _reject.insert( opt.reject.begin(), opt.reject.end() );

  for (const auto& r: opt.rules)
  {
    auto target = this->get_adapter(r.target);
    _targets.push_back( target );
    /*for (const auto& m: r.methods)
      _methods[m] = target;*/
  }
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
    ::wjrpc::aux::send_error(std::move(holder), std::make_unique< ::wjrpc::service_unavailable > (), std::move(handler));
    return;
  }
  
  /*
  auto itr = _methods.find(holder.method());
  if ( itr != _methods.end() )
  {
    itr->second.perform_incoming(std::move(holder), io_id, std::move(handler) );
    return;
  }*/
    
  if ( this->get_target() ) 
  {
    domain_proxy::perform_incoming(std::move(holder), io_id, std::move(handler) );
    return;
  }

  ::wjrpc::aux::send_error(std::move(holder),  std::make_unique< ::wjrpc::procedure_not_found > (), std::move(handler));
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
