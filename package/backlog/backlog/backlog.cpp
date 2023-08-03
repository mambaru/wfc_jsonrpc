#include "backlog.hpp"
#include "ibacklog.hpp"
#include <wfc/logger.hpp>
#include <wfc/memory.hpp>
#include <sstream>
#include <boost/filesystem.hpp>
#include <iostream>

namespace wfc{ namespace jsonrpc{

#define BACKLOG(NAME, X) WFC_LOG_MESSAGE(NAME, X)
namespace
{

  class backlog_proxy: public ibacklog
  {
  public:
    explicit backlog_proxy(backlog* bl) noexcept
      : _backlog(bl)
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

    virtual size_t restore() override
    {
      return _backlog->restore();
    }

    void async_restore(std::function<void(size_t)> cb)
    {
      _backlog->async_restore(cb);
    }

    bool activate()
    {
      return _backlog->activate();
    }

    bool deactivate()
    {
      return _backlog->deactivate();
    }

    void restore_cancel()
    {
      return _backlog->restore_cancel();
    }

    // iinterface
    virtual void reg_io( io_id_t io_id, std::weak_ptr<iinterface> itf) override
    {
      if ( auto src = _source.lock() )
        src->reg_io(io_id, itf);

      // Вызывается только в клиент режиме
      COMMON_LOG_MESSAGE("JSON-RPC backlog target available. Restore from backlog")
      _backlog->deactivate();
      _backlog->async_restore([this](size_t count){
          COMMON_LOG_MESSAGE("JSON-RPC restored from backlog. Restored records: " << count)
          this->_backlog->rotate();
      });

    }

    virtual void unreg_io(io_id_t io_id) override
    {
      if ( auto src = _source.lock() )
        src->unreg_io(io_id);
      // Вызывается только в клиент режиме
      COMMON_LOG_MESSAGE("JSON-RPC backlog target not available. Backlog ebabled")
      _backlog->activate();
    }


    void set_source(std::weak_ptr<iinterface> src)
    {
      _source = src;
    }

  private:
    backlog* _backlog = nullptr;
    std::weak_ptr<iinterface> _source;
 };

}

backlog::backlog()
  : _counter(0)
  , _client_mode(false)
  , _active(true)
  , _restore_cancel(false)
{
}

void backlog::configure()
{
  domain_proxy::configure();
  _source_id = this->get_id();
  _log = this->options().log;
  _client_mode = this->options().mode == backlog_mode::client;
  _active = !_client_mode;
  this->reg_object("backlog", this->name(), std::make_shared<backlog_proxy>(this) );
}

void backlog::initialize()
{
  domain_proxy::initialize();
  _filelog.open(this->options().path, std::ofstream::out | std::ofstream::app);
}

void backlog::start()
{
  domain_proxy::start();
  _counter = 0;
/*  if ( _client_mode )
  {
    this->get_target().reg_io( this->get_id(), this->get_object<iinterface>("backlog", this->name()) );
  }*/
}

void backlog::reg_io( io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  _source_id = io_id;
  if ( _client_mode )
  {
    COMMON_LOG_MESSAGE("JSON-RPC backlog::reg_io " << io_id << " self::io_id:" << this->get_id() )
    auto bl = this->get_object<backlog_proxy>("backlog", this->name());
    bl->set_source(itf);
    domain_proxy::reg_io(io_id, bl);
  }
  else
  {
    domain_proxy::reg_io(io_id, itf);
  }
}


void backlog::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler)
{
  if ( _active )
  {
    if ( holder.is_request() || holder.is_notify() )
      this->write_incoming_( holder);
  }
  domain_proxy::perform_incoming( std::move(holder), io_id, handler);
}

void backlog::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  //COMMON_LOG_MESSAGE("JSON-RPC backlog::perform_outgoing " << io_id)

  if ( _active )
  {
    if ( holder.is_request() || holder.is_notify() )
    {
      outgoing_holder oholder = holder.clone(_counter);
      incoming_holder iholder( oholder.detach() );
      iholder.parse(nullptr);
      this->write_incoming_( iholder );
    }
  }

  domain_proxy::perform_outgoing( std::move(holder), io_id);
}

void backlog::write_incoming_(const incoming_holder& holder)
{
  if ( !this->suspended() )
  {
    std::string jsonmsg = holder.str();
    std::lock_guard<mutex_type> lk(_mutex);

    if ( !_log.empty() )  { BACKLOG(_log,jsonmsg) }

    if ( _lock_flag )
      _ss << jsonmsg << std::endl;
    else
      _filelog << jsonmsg << std::endl;
  }
}

size_t backlog::apply_backlog_()
{
  auto opt = this->options();
  if ( !boost::filesystem::exists(opt.path) )
    return 0;

  size_t ready_count = 0;
  std::ifstream filelog(opt.path);


  target_adapter next = this->get_adapter(opt.restore_target, true);
  if ( !next )
  {
    COMMON_LOG_WARNING("JSON-RPC backlog '" << this->name() << "' is using main target. "
      << "You may set another target with 'restore_target' property " )
  }

  _restore_cancel = false;
  while ( filelog )
  {
    std::string json;
    while ( std::getline(filelog, json) )
    {
      if ( _restore_cancel || this->global_stop_flag() )
        return ready_count;

      incoming_holder holder( json );
      json::json_error er;
      holder.parse(&er);
      if ( !er )
      {
        if ( next )
          next.perform_incoming( std::move(holder), _source_id, nullptr);
        else
          domain_proxy::perform_incoming( std::move(holder), _source_id, [](outgoing_holder){
            std::cout <<  "\n\nabort();" << std::endl;
          });
        std::cout << json << std::endl;
        ++ready_count;
      }
      else
      {
        COMMON_LOG_ERROR("Restore jsonrpc::backlog JSON error: " << json::strerror::message_trace(er, json.begin(), json.end() )  )
      }

      if ( opt.restore_trace!=0 && ((ready_count%opt.restore_trace)==0) )
      {
        COMMON_LOG_PROGRESS("JSON-RPC backlog restored " << ready_count << " records. " )
      }
    }
  }
  filelog.close();
  return ready_count;
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
  _filelog.open(opt.path, std::ofstream::out | std::ofstream::trunc);
}

size_t backlog::restore()
{
  std::lock_guard<mutex_type> lk(_mutex);
  _lock_flag = false;
  _filelog << _ss.str();
  return this->apply_backlog_();
}

void backlog::async_restore(std::function<void(size_t)> cb)
{
  this->get_workflow()->safe_post(
    /*std::chrono::seconds(10),*/
    _restore_owner.wrap([this, cb](){
      size_t count = this->restore();
      if ( cb != nullptr )
        cb(count);
    },
    [](){
      COMMON_LOG_MESSAGE("JSON-RPC backlog restore canceled. " )
    })
  );
}

void backlog::restore_cancel()
{
  _restore_cancel = true;
  _restore_owner.reset();
}

bool backlog::activate()
{
  if ( _active )
    return false;
  _active = true;
  return true;
}

bool backlog::deactivate()
{
  if ( !_active )
    return false;
  _active = false;
  return true;
}

}}
