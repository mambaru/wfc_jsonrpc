#pragma once

#include "broker_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/mutex.hpp>
#include <matchmaker/matchmaker.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{ 
  
class broker
  : public ::wfc::jsonrpc::domain_proxy<broker_config, wfc::nostat>
{
  typedef ::wfc::jsonrpc::domain_proxy<broker_config, wfc::nostat> super;
  
public:
  typedef super::domain_config domain_config;
  virtual domain_config generate(const std::string&);

  virtual void configure() override;
  virtual void reconfigure() override;
  // domain_proxy
  virtual void restart() override;
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override;
  virtual void unreg_io(io_id_t io_id) override;

  // ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;

private:
  
  struct rule_target
  {
    std::set<std::string> methods;
    mutable target_adapter target;
    std::shared_ptr<matchmaker> matcher;
    std::string rule_log;
  };
 
  typedef std::list<target_adapter> target_list;
  
  typedef std::set<std::string> reject_list;
  typedef rwlock<std::recursive_mutex> mutex_type;
  typedef std::vector<rule_target> rule_list;

  target_list  _targets;
  reject_list  _reject;
  rule_list    _rules;
  
  std::string _target_log;
  std::string _reject_log;


  mutable mutex_type _mutex;
  bool _reconf_flag = false;
  bool _reject_all = false;
};

}}
