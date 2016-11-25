#pragma once

#include "statistics_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/statistics/multi_meter.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{ 

class statistics
  : public ::wfc::jsonrpc::domain_proxy< statistics_config >
{
  typedef multi_meter meter_type;
  typedef std::shared_ptr<meter_type> meter_ptr;
  typedef std::map< std::string, meter_ptr > meter_map;

public:
  virtual void reconfigure() override;
  // ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, rpc_outgoing_handler_t handler) override;
private:
  meter_ptr request_meter_(std::string name, size_t size);
  meter_ptr notify_meter_(std::string name, size_t size);
  meter_ptr other_meter_(size_t size);
  meter_ptr create_meter_(meter_map& meters, size_t size);
private:
  meter_map _req_meters;
  meter_map _ntf_meters;
  meter_ptr  _other;
  std::mutex _mutex;
};

}}
