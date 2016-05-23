#pragma once

#include "jsonrpc_adapter.hpp"

#include <wfc/domain_object.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>

#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
template<typename Config>
class domain_base
  : public ::wfc::domain_object< ijsonrpc, Config>
{
  typedef ::wfc::domain_object< ijsonrpc, Config> super;
  typedef std::weak_ptr<ijsonrpc> jsonrpc_wptr;
public:
  typedef typename super::io_id_t  io_id_t;
  typedef typename super::data_ptr  data_ptr;
  typedef typename super::io_outgoing_handler_t  io_outgoing_handler_t;
  typedef typename super::rpc_outgoing_handler_t  rpc_outgoing_handler_t;
  typedef typename super::incoming_holder incoming_holder;
  typedef typename super::outgoing_holder outgoing_holder;
  
  domain_base() {}
  
  jsonrpc_adapter get_adapter(  const std::string& name ) const
  {
    auto itf = this->template get_target< iinterface >( name );
    auto jrpc = this->template get_target< ijsonrpc >( name, true );
    return jsonrpc_adapter(itf, jrpc);
  }
  
  virtual void reconfigure() override
  {
    _target = this->get_adapter( this->options().target );
  }
  
// iinterface
  virtual void reg_io( io_id_t io_id, std::weak_ptr<iinterface> itf) override
  {
    _target.reg_io( io_id, itf );
  }
  
  virtual void unreg_io(io_id_t io_id) override
  {
    _target.unreg_io( io_id);
  }
  
  virtual void perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t handler) final override
  {
    if ( this->suspended() )
    {
      _target.perform_io( std::move(d), io_id, handler );
      return;
    }
    
    // проверить исключения jsonrpc
    while ( d != nullptr )
    {
      incoming_holder holder( std::move(d), false);
      d = holder.parse();
      this->perform_incoming( std::move(holder), io_id, [handler](outgoing_holder holder)
      {
        handler( holder.detach() );
      } );
    }
  }
  
// ijsonrpc
  virtual void perform_incoming( incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) override
  {
    _target.perform_incoming( std::move(holder), io_id, handler);
  }
  
  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) override
  {
    
    _target.perform_outgoing( std::move(holder), io_id);
  }
  
  jsonrpc_adapter target() const { return _target;}
  
public:
  jsonrpc_adapter _target;
};

}}
