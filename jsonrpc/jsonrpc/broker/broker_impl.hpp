#pragma once

#include "broker.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <iow/jsonrpc/incoming/aux.hpp>

namespace wfc{

class broker_impl
  : public ijsonrpc
{
  typedef std::shared_ptr<ijsonrpc> target_ptr;
  typedef std::set<target_ptr> target_set;
  typedef std::map<std::string, target_ptr> method_map;
  typedef std::mutex mutex_type;
  typedef std::set<std::string> reject_list;
public:
  
  void reconfigure(const broker_config& opt, const ::wfc::object_registry& reg)
  {
    _reject.insert( opt.reject.begin(), opt.reject.end() );
    
    for (const auto& o: opt.targets)
    {
      auto ptarget = reg.get<ijsonrpc>(o.target);
      _targets.insert( ptarget );
      for (const auto& m: o.methods)
      {
        _methods[m] = ptarget;
      }
    }
    _other = reg.get<ijsonrpc>(opt.target);
  }
  
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override
  {
    for (auto& t : _targets )
    {
      t->reg_io(io_id, itf);
    }
  }

  virtual void unreg_io(io_id_t io_id) override
  {
    for (auto& t : _targets )
    {
      t->unreg_io(io_id);
    }
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t handler) override
  {
    using namespace std::placeholders;
    /*
    ::iow::jsonrpc::incoming_holder::perform(std::move(d), io_id, handler, std::bind(&broker_impl::perform_incoming, this, _1, _2, _3) );
    */
#warning TODO: сделать
    abort();
  }

  virtual void perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) override
  {
    if ( !holder.has_method() )
    {
      if ( _other != nullptr ) 
      {
        _other->perform_incoming(std::move(holder), io_id, std::move(handler) );
      }
      return;
    }
    
    if ( _reject.find( holder.method() ) != _reject.end() )
    {
      ::iow::jsonrpc::aux::send_error(std::move(holder), std::make_unique< ::iow::jsonrpc::service_unavailable > (), std::move(handler));
      return;
    }
    
    auto itr = _methods.find(holder.method());
    if ( itr != _methods.end() )
    {
      itr->second->perform_incoming(std::move(holder), io_id, std::move(handler) );
      return;
    }
    
    if ( _other != nullptr ) 
    {
      _other->perform_incoming(std::move(holder), io_id, std::move(handler) );
      return;
    }

    ::iow::jsonrpc::aux::send_error(std::move(holder), std::make_unique< ::iow::jsonrpc::procedure_not_found > (), std::move(handler));
    
  }
    
  virtual void perform_outgoing(outgoing_holder holder, io_id_t io_id) override
  {
    if ( _other != nullptr )
    {
      _other->perform_outgoing( std::move(holder), io_id);
    }
  }
  
private:
  
  target_ptr  _other;
  target_set  _targets;
  method_map  _methods;
  reject_list _reject;
};

}
