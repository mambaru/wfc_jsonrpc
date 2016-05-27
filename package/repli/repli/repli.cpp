#include "repli.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <wfc/jsonrpc/target.hpp>

namespace wfc{ namespace jsonrpc{ 

  /*
class repli::impl
{
public:

  impl( const repli_config& conf, ::wfc::wfcglobal::ptr g)
  {
    _primary = std::make_shared<target>( conf.target, g);
    for (const auto& c: conf.reply_targets )
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

  void perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
  {
    if ( _targets.empty() )
    {
      _primary->perform_incoming( std::move(holder), io_id, std::move(handler) );
      return;
    }
    else
    {
      _primary->perform_incoming( holder.clone(), io_id, handler );
    }

    auto last = (++_targets.rbegin()).base();
    for ( auto itr = _targets.begin(); itr!=last; ++itr )
    {
      (*itr)->perform_incoming( holder.clone(), io_id, handler );
    }

    (*last)->perform_incoming( std::move(holder), io_id, std::move(handler) );
  }

  void perform_outgoing(outgoing_holder holder, io_id_t io_id)
  {
    if ( _targets.empty() )
    {
      _primary->perform_outgoing( std::move(holder), io_id );
      return;
    }
    else
    {
      _primary->perform_outgoing( holder.clone(), io_id );
    }

    auto last = (++_targets.rbegin()).base();
    for ( auto itr = _targets.begin(); itr!=last; ++itr )
    {
      (*itr)->perform_outgoing( holder.clone(), io_id );
    }

    (*last)->perform_outgoing( std::move(holder), io_id );
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
*/

void repli::reconfigure()
{
  domain_proxy::reconfigure();
  for ( auto name : this->options().reply_targets )
  {
    _targets.push_back(  this->get_adapter(name) );
  }
}

void repli::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  for ( auto& r : _targets )
    r.perform_incoming( holder.clone(), io_id, handler);
  domain_proxy::perform_incoming( std::move(holder), io_id, handler );
}

void repli::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  for ( auto& r : _targets )
    r.perform_outgoing( holder.clone() , io_id);
  domain_proxy::perform_outgoing( std::move(holder), io_id);
}


}}