#include "broker.hpp"
#include <wjrpc/incoming/send_error.hpp>
#include <wfc/logger.hpp>

#define BROKER_LOG(NAME, X) if (!NAME.empty()) { WFC_LOG_MESSAGE(NAME, X) }

namespace wfc{ namespace jsonrpc{ 


broker::domain_config broker::generate(const std::string& val)
{
  domain_config conf = super::generate(val);
  if ( !val.empty() )
  {
    using namespace wfc::json::literals;
    conf.reject.push_back("<<reject-method-name>>");
    
    domain_config::rule r;
    r.target = "<<target-name>>";
    r.methods.insert("<<method-name>>");
    r.methods.insert(
      "В этом массиве перечисляются все методы, которые будут перенаправлены объекту указанному в 'target'."
      "Это строка комментарий вместо реального имени метода, ее нужно удалить!"
    );
    conf.rules.push_back(r);
    
    r.mode = match_mode::FullMatch;
    r.methods.clear();
    r.methods.insert("method1");
    r.methods.insert("Это правило выполнится для 'method1' если в параметрах есть поле 'arg1' с любым значением");
    r.params = std::make_shared<std::string>("{'arg1':null}"_json);
    conf.rules.push_back(r);

    r.methods.clear();
    r.methods.insert("method2");
    r.methods.insert("Это правило выполнится для 'method2' если в параметрах есть поле 'arg2' со значением 'value2'");
    r.params = std::make_shared<std::string>("{'arg2':'value2'}"_json);
    conf.rules.push_back(r);
    
    r.methods.clear();
    r.methods.insert("method3");
    r.methods.insert("В режиме PrefixMatch проверяется только первая часть значения или имени. В 'method3' примере  "
      "подойдет любое поле которое начинается с 'arg' со значениями начинающиеся value"
    );
    r.mode = match_mode::FullMatchName | match_mode::PrefixMatchValue;
    r.params = std::make_shared<std::string>("{'arg':'value'}"_json);

    r.methods.clear();
    r.methods.insert("method4");
    r.methods.insert("Можно использовать регулярные выражение в режиме RegexMatch. Обратите внимание, что регулярное "
    "выражение должно описывать json-в сыром виде, например строка будет с кавычками");
    conf.rules.push_back(r);
    r.mode = match_mode::RegexMatch;
    r.params = std::make_shared<std::string>("{'.*':'.(\\\\w+)(\\\\.|_)?(\\\\w*)@(\\\\w+)(\\\\.(\\\\w+))+.'}"_json);
    conf.rules.push_back(r);
  }
  return conf;
}

void broker::configure() 
{
  _reconf_flag = false;
}

void broker::reconfigure()
{
  _reconf_flag = true;
}

void broker::restart()
{
  target_list  targets;
  reject_list  reject;
  rule_list    rules;

  const auto& opt = this->options();
  reject.insert( opt.reject.begin(), opt.reject.end() );
  targets.clear();

  std::set<std::string> names;
  for (const broker_config::rule& r: opt.rules)
  {
    if (  !r.target.empty() )
      names.insert(r.target);
  }
  
  for (const std::string& n: names)
  {
    targets.push_back( this->get_adapter(n) );
  }
  
  for (const broker_config::rule& r: opt.rules)
  {

    rules.push_back(rule_target());
    rules.back().methods = r.methods;
    rules.back().target = this->get_adapter(r.target);
    rules.back().rule_log = r.rule_log;
    
    if ( r.params!=nullptr && !r.params->empty() )
    {
      rules.back().matcher = std::make_shared<matchmaker>();
      json::json_error err;
      if ( !rules.back().matcher->reconfigure(r.mode, *r.params, err) )
      {
        if ( _reconf_flag ) 
        {
          COMMON_LOG_ERROR( "jsonrpc-broker configuration error: " <<  json::strerror::message_trace(err, r.params->begin(), r.params->end()) )
        }
        else
        {
          COMMON_LOG_FATAL( "jsonrpc-broker configuration error: " <<  json::strerror::message_trace(err, r.params->begin(), r.params->end()) )
        }
        return;
      }
    }
  }
  
  std::lock_guard<mutex_type> lk(_mutex);
  _targets = targets;
  _reject = reject;
  _rules = rules;
  _target_log = opt.target_log;
  _reject_log = opt.reject_log;
  _reject_all = _reject.count("*");
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
  if ( !_reject.empty() )
  {
    if ( _reject_all || _reject.count( holder.method() ) != 0 )
    {
      BROKER_LOG(_reject_log, holder.str())
      this->send_error< wjrpc::procedure_not_found >(std::move(holder), std::move(handler));
      return;
    }
  }
  
  for (const rule_target& r: _rules)
  {
    bool all_methods = r.methods.empty() || r.methods.count("*")!=0;
    if ( 0 != r.methods.count(holder.method()) || all_methods )
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
        BROKER_LOG(r.rule_log, holder.str())
        r.target.perform_incoming(std::move(holder), io_id, std::move(handler) );
        return;
      }
    }
  }
    
  BROKER_LOG(_target_log, holder.str())
  domain_proxy::perform_incoming(std::move(holder), io_id, std::move(handler) );
}
  
void broker::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  domain_proxy::perform_outgoing(std::move(holder), io_id );
}


}}
