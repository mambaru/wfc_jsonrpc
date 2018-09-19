#pragma once

#include "broker_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/mutex.hpp>
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

  // domain_proxy
  virtual void ready() override;
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override;
  virtual void unreg_io(io_id_t io_id) override;

  // ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;

private:
  typedef broker_config::rule rule_type; 
  struct rule_target: rule_type
  {
    std::shared_ptr<target_adapter> target;
    bool match_params(const char* beg, const char* end, json::json_error& er);
    bool match_fields(const std::string& rawjsonconf, const char* beg, const char* end, json::json_error& er);
  };
 
  typedef std::list<target_adapter> target_list;
  //typedef std::map<std::string, target_adapter> method_map;
  typedef std::set<std::string> reject_list;
  typedef rwlock<std::mutex> mutex_type;
  typedef std::vector<rule_target> rule_list;

  target_list  _targets;
  //method_map   _methods;
  rule_list _rule_list;
  reject_list  _reject;

  mutable mutex_type _mutex;

};

}}
