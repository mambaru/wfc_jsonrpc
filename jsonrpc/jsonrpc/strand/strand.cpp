#include "strand.hpp"
#include "strand_options.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <iow/io/basic_io.hpp>
#include <cassert>
#include <thread>

namespace wfc{

struct _strand_context_;
struct strand_context
{
  typedef ::iow::asio::io_service io_service_type;
  typedef io_service_type::strand strand_type;
  typedef std::shared_ptr<strand_type> strand_ptr;
  
  typedef std::atomic<size_t> counter_type;
  typedef std::shared_ptr<counter_type> counter_ptr;
  std::string name;
  counter_ptr counter;
  strand_ptr strand;
};

struct strand_aspect: public fas::aspect
<
>{};

class basic_strand
  : public ::iow::io::basic_io< /*fas::aspect< fas::stub< ::iow::io::_initialize_> >*/ >
  , public std::enable_shared_from_this<basic_strand>
{
public:
  typedef ::iow::io::basic_io<> super;
  typedef ::iow::asio::io_service io_service_type;

  basic_strand(io_service_type& io)
    : _strand(io)
  {
    _counter = std::make_shared<counter_type>(0);
  }
  
  template<typename F>
  void post(F&& f)
  {
    _strand.post( this->wrap( std::forward<F>(f)) );
  }

  template<typename F>
  std::function<void()> 
  wrap(F&& f)
  {
    std::weak_ptr<basic_strand> wthis = this->shared_from_this();
    ++( *_counter );
    
    auto f1 = [f, wthis]()
    {
      if ( auto pthis = wthis.lock() )
      {
        auto& c = *(pthis->_counter);
        assert( c > 0 );
        std::cout << "================================================================== > " << c << std::endl;
        --c;
        f();
      }
    };
    
    return std::move( super::wrap( std::move(f1) ) ) ;
  }
  
  size_t size() const
  {
    return *_counter;
  }

private:
  typedef std::atomic<size_t> counter_type;
  std::shared_ptr<counter_type> _counter;
  io_service_type::strand _strand;
};

class mt_strand
{
public:
  typedef basic_strand strand_type;
  typedef std::shared_ptr<strand_type> strand_ptr;
  typedef std::list<strand_ptr> strand_list;
  typedef ::iow::asio::io_service io_service_type;
  typedef std::shared_ptr<io_service_type> io_service_ptr;
  typedef std::mutex mutex_type;

  mt_strand(io_service_type& io)
    : _io_ref(io)
  {
  }
  
  template<typename Opt>
  void start(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _wrntime = 0;
    _maxsize = opt.maxsize;
    _wrnsize = opt.wrnsize;
    if ( opt.threads != 0 )
    {
      _io_ptr = std::make_shared<io_service_type>();
    }
    else
    {
      _io_ptr = nullptr;
    }
    
    for ( size_t i = 0; !opt.disabled && i < opt.strands; ++i )
    {
      auto s = this->create_();
      //s->start(std::forward<Opt>(opt));
      _strands.push_back( s );
    }
    
    if ( !_strands.empty() )
    {
      for ( size_t i = 0; i < opt.threads; ++i )
      {
        _threads.push_back(std::thread([this]()
        {
          io_service_type& io = *( this->_io_ptr );
          io_service_type::work wrk(io);
          io.run();
          IOW_LOG_MESSAGE("strand thread finish")
        }));
      }
    }
  }
  
  void stop()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if (_io_ptr!=nullptr)
    {
      _io_ptr->stop();
    }
    
    for ( auto s : _strands ) 
    {
      s->stop();
    }
    
    for ( std::thread& t : _threads ) 
    {
      t.join();
    }
  }
  
  bool enabled() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return !_strands.empty();
  }
  
  template<typename F>
  bool post(F&& f)
  {
    std::cout << "incoming::post -0-" << std::endl;
    std::lock_guard<mutex_type> lk(_mutex);
    std::cout << "incoming::post -1-" << std::endl;
    if ( _strands.empty() )
    {
      std::cout << "incoming::post -2-" << std::endl;
      f();
      std::cout << "incoming::post -3-" << std::endl;
      return false;
    }
    
    typedef strand_list::iterator iterator;
    iterator itr = _strands.begin();
    auto beg = itr;
    auto end = _strands.end();
    for ( ++beg ;beg!=end; ++beg)
    {
      if ( (*beg)->size() < (*itr)->size() )
      {
        itr = beg;
      }
    }
    
    std::cout << "incoming::post -4-" << std::endl;
    size_t size = (*itr)->size();
    std::cout << "incoming::post -5-" << std::endl;
    if ( size >= _maxsize )
    {
      IOW_LOG_ERROR("mt_strand queue overflow size=" << size << " ( maxsize = " << _maxsize << ")")
      abort();
      return false;
    }
    else if ( size > _wrnsize )
    {
      time_t now = time(0);
      if ( now != _wrntime )
      {
        IOW_LOG_WARNING("mt_strand queue overflow warning size = " << size << "( wrnsize=" << _wrnsize << ")")
        _wrntime = now;
      }
    }
    std::cout << "incoming::post -6-" << std::endl;
    (*itr)->post( std::forward<F>(f));
    std::cout << "incoming::post -7-" << std::endl;
    return true;
  }

  
  size_t size() const
  {
    size_t size = 0;
    for (auto &s : _strands)
    {
      size += s->size();
    }
    return size;
  }
  
private:
  strand_ptr create_()
  {
    if ( _io_ptr!=nullptr ) return std::make_shared< strand_type>(*_io_ptr);
    else return std::make_shared< strand_type>(_io_ref);
  }
  
private:
  typedef std::list<std::thread> thread_list;
  thread_list _threads;
  strand_list _strands;
  io_service_type& _io_ref;
  io_service_ptr _io_ptr;
  size_t _maxsize;
  size_t _wrnsize;
  time_t _wrntime;
  mutable mutex_type _mutex;
};


/*
 *   virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) = 0;
  virtual void perform_outgoing(outgoing_holder, io_id_t ) = 0;
 */
class jsonrpc_strand
  : public ijsonrpc
  , public std::enable_shared_from_this<jsonrpc_strand>
{
  typedef mt_strand strand_type;
  typedef std::shared_ptr<strand_type> strand_ptr;
  typedef std::recursive_mutex mutex_type;
public:
  
  typedef strand_type::io_service_type io_service_type;
  
  jsonrpc_strand(io_service_type& io)
  {
    _incoming = std::make_shared<strand_type>(io);
    _outgoing = std::make_shared<strand_type>(io);
  };
  
  void start(strand_options opt1, strand_options opt2, std::shared_ptr<ijsonrpc> target)
  {
    _incoming->start(opt1);
    _outgoing->start(opt2);
    _target = target;
  }
  
  void stop()
  {
    _incoming->stop();
    _outgoing->stop();
  }
  
  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t h) override
  {
    auto handler = make_handler_( std::move(h) );
    
    if ( _incoming->enabled() )
    {
      auto pholder = std::make_shared<incoming_holder>( std::move(holder) );
      auto ptarget = _target;
      _incoming->post([ptarget, pholder, io_id, handler]()
      {
        ptarget->perform_incoming( std::move(*pholder), io_id, handler);
      });
    }
    else if ( handler != nullptr )
    {
      handler(nullptr);
    }
  }
  
  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) override
  {
    if ( _incoming->enabled() )
    {
      auto pholder = std::make_shared<outgoing_holder>( std::move(holder) );
      auto ptarget = _target;
      _incoming->post([ptarget, pholder, io_id]()
      {
        ptarget->perform_outgoing( std::move(*pholder), io_id);
      });
    }
    
  }
  
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
  {
    _target->reg_io(io_id, itf);
  }

  virtual void unreg_io(io_id_t io_id)
  {
    _target->unreg_io(io_id);
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t h)
  {
    std::cout << "make_handler_... " << d << std::endl;
    auto handler = make_handler_( std::move(h) );
    std::cout << "make_handler_... ready" << std::endl;

    if ( _incoming->enabled() )
    {
      auto pd = std::make_shared<data_ptr>( std::move(d) );
      auto ptarget = _target;
      std::cout << "==1==" << std::endl;
      _incoming->post([ptarget, pd, io_id, handler]()
      {
        std::cout << "--1--" << std::endl;
        ptarget->perform_io( std::move(*pd), io_id, handler);
        std::cout << "--2--" << std::endl;
      });
      std::cout << "==2==" << std::endl;
    }
    else if ( handler != nullptr )
    {
      handler(nullptr);
    }
    
  }
  
private:
  
  
  outgoing_handler_t make_handler_(outgoing_handler_t handler)
  {
    std::cout << "make_handler_ 1" << std::endl;
    if ( handler!=nullptr && _outgoing->enabled() )
    {
      std::cout << "make_handler_ 2" << std::endl;
      std::weak_ptr<jsonrpc_strand> wthis = this->shared_from_this();
      std::cout << "make_handler_ 3" << std::endl;
      outgoing_handler_t h = [wthis, handler]( data_ptr d)
      {
        std::cout << "make_handler_ call  " << d << std::endl;
        if ( auto pthis = wthis.lock() )
        {
          auto pd = std::make_shared<data_ptr>( std::move(d));
          std::cout << "make_handler_ post ..." << std::endl;
          std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++1> " << pthis->_outgoing->size() << std::endl;
          pthis->_outgoing->post([pd, handler](){
            std::cout << "make_handler_ READY" << std::endl;
            handler(std::move(*pd));
          });
          std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++2>" << pthis->_outgoing->size() << std::endl;
          std::cout << "make_handler_ posted" << std::endl;
        }
      };
      std::cout << "make_handler_ 4" << std::endl;
      auto h1 =  std::move(h);
      std::cout << "make_handler_ 5" << std::endl;
      return std::move(h1);
    }
    return std::move(handler);
  }
  
private:
  
  strand_ptr _incoming;
  strand_ptr _outgoing;
  std::shared_ptr<ijsonrpc> _target;
};
  
class strand::impl: public jsonrpc_strand
{
public:
  impl( ::wfc::asio::io_service& io)
    : jsonrpc_strand(io)
  {}
};

strand::~strand()
{
}

strand::strand()
{
}

void strand::reconfigure()
{
#warning Сделать нормальный реконфиг
  if ( auto g = this->global() )
  {
    const auto& conf = this->options();
    auto target = this->global()->registry.get<ijsonrpc>(conf.target);
    if (_impl == nullptr)
    {
      _impl = std::make_shared<strand::impl>( g->io_service );
    }
    _impl->start( conf.incoming, conf.outgoing, target);
  }

}

void strand::stop(const std::string&) 
{
  if ( _impl != nullptr )
  {
    _impl->stop();
  }
}


void strand::start(const std::string&)
{
  /*
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<strand::impl>( g->io_service );
    _impl->start();
  }
  */
}


void strand::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  if ( _impl != nullptr )
  {
    _impl->reg_io(io_id, itf);
  }
}

void strand::unreg_io(io_id_t io_id)
{
  if ( _impl != nullptr )
  {
    _impl->unreg_io(io_id);
  }
}

void strand::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
{
  if ( _impl != nullptr )
  {
    _impl->perform_io( std::move(d), io_id, std::move(handler) );
  }
}

void strand::perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
{
  if ( _impl != nullptr )
  {
    _impl->perform_incoming( std::move(holder), io_id, std::move(handler) );
  }
}
  
void strand::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( _impl != nullptr )
  {
    _impl->perform_outgoing( std::move(holder), io_id );
  }
}


}
