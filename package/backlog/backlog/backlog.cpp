#include "backlog.hpp"
#include "ibacklog.hpp"
#include <wfc/logger.hpp>
#include <wfc/memory.hpp>
#include <sstream>
#include <boost/filesystem.hpp>

namespace wfc{ namespace jsonrpc{

namespace
{
  
  class backlog_proxy: public ibacklog
  {
  public:
    explicit backlog_proxy(backlog* bl): _backlog(bl)
    {
    }
    
    virtual void rotate() override
    {
      _backlog->rotate();
    }
    
    virtual bool lock() override
    {
      return _backlog->lock();
    }

    virtual bool unlock() override
    {
      return _backlog->unlock();
    }
    
    virtual void restore() override
    {
      _backlog->restore();
    }
  private:
    backlog* _backlog = nullptr;
  };

}

void backlog::start()
{
  this->set_target("backlog", this->name(), std::make_shared<backlog_proxy>(this) );
  
}
  
void backlog::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
{
  if ( holder.is_request() || holder.is_notify() )
    this->write_incoming_( holder);
  domain_proxy::perform_incoming( std::move(holder), io_id, handler);
}
  
void backlog::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( holder.is_request() || holder.is_notify() )
  {
    outgoing_holder oholder = holder.clone();
    incoming_holder iholder( oholder.detach() );
    iholder.parse(nullptr);
    this->write_incoming_( iholder );
  }
  
  domain_proxy::perform_outgoing( std::move(holder), io_id);
}

void backlog::write_incoming_(const incoming_holder& holder)
{
  if ( !this->suspended() )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _lock_flag )
      _ss << holder.str() << std::endl;
    else
      _filelog << holder.str() << std::endl;
  }
}

void backlog::apply_backlog_()
{
  auto opt = this->options();
  if ( !boost::filesystem::exists(opt.path) )
    return;
  
  std::ifstream filelog(opt.path);
  
  while ( filelog )
  {
    std::string json;
    while ( std::getline(filelog, json) )
    {
      incoming_holder holder( json );
      json::json_error er;
      holder.parse(&er);
      if ( !er )
      {
        domain_proxy::perform_incoming( std::move(holder), this->get_id(), nullptr);
      }
      else
      {
        COMMON_LOG_ERROR("Restore backlog JSON error: " << json::strerror::message_trace(er, json.begin(), json.end() )  )
      }
        
    }
  }
  filelog.close();
}



bool backlog::lock()
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _lock_flag) return false;
  _lock_flag = true;
  return true;
}

bool backlog::unlock()
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( !_lock_flag) return false;
  _lock_flag = false;
  _filelog << _ss.str();
  return true;
}

void backlog::rotate()
{
  auto opt = this->options();
  std::lock_guard<mutex_type> lk(_mutex);
  _filelog.close();
  _filelog.open(opt.path);
}

void backlog::restore()
{
  std::lock_guard<mutex_type> lk(_mutex);
  _lock_flag = false;
  _filelog << _ss.str();
  this->apply_backlog_();
}

}}
