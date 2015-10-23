#include "repli.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <wfc/jsonrpc/target.hpp>

namespace wfc{ namespace jsonrpc{ namespace repli{

class repli::impl
{
public:

  impl( const repli_config& conf, ::wfc::wfcglobal::ptr g)
  {
    _primary = std::make_shared<target>( conf.primary_target, g);
    for (const auto& c: conf.targets )
    {
      _targets.push_back( std::make_shared<target>(c, g) );
    }
  }

  void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
  {
    _primary->reg_io(io_id, itf);
    for (const auto& p : _targets)
    {
      p->reg_io(io_id, itf);
    }
  }

  void unreg_io(io_id_t io_id)
  {
    _primary->unreg_io(io_id);
    for (const auto& p : _targets)
    {
      p->unreg_io(io_id);
    }
  }

  void perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t handler) 
  {
    if ( _targets.empty() )
    {
      _primary->perform_io( std::move(d), io_id, std::move(handler) );
      return;
    }
    else
    {
      _primary->perform_io( std::make_unique<data_type>(*d), io_id, handler );
    }

    auto last = (++_targets.rbegin()).base();
    for ( auto itr = _targets.begin(); itr!=last; ++itr )
    {
      (*itr)->perform_io( std::make_unique<data_type>(*d), io_id, handler );
    }

    (*last)->perform_io( std::move(d), io_id, std::move(handler) );
  }

  void perform_incoming(incoming_holder , io_id_t, rpc_outgoing_handler_t ) 
  {
//#error TODO: holder.clone()
    DEBUG_LOG_MESSAGE("repli::perform_incoming")
    abort();
  }

  void perform_outgoing(outgoing_holder , io_id_t )
  {
//#error TODO: holder.clone()
    DEBUG_LOG_MESSAGE("repli::perform_outgoing")
    abort();
  }

private:

  typedef std::shared_ptr<target> target_ptr;
  typedef std::vector<target_ptr> target_list;

  target_ptr _primary;
  target_list _targets;
};

repli::~repli()
{
}

repli::repli()
{
}

void repli::reconfigure()
{
  _impl = std::make_shared<repli::impl>( this->options(), this->global() );
}

void repli::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  _impl->reg_io(io_id, itf);
}

void repli::unreg_io(io_id_t io_id)
{
  _impl->unreg_io(io_id);
}

void repli::perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t handler) 
{
  _impl->perform_io(std::move(d), io_id, std::move(handler) );
}

void repli::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  _impl->perform_incoming(std::move(holder), io_id, std::move(handler) );
}

void repli::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  _impl->perform_outgoing(std::move(holder), io_id);
}


}}}
