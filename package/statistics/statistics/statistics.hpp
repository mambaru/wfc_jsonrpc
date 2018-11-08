#pragma once

#include "statistics_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/statistics/meters.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{ 

class statistics
  : public ::wfc::jsonrpc::domain_proxy< statistics_config, statistics_stat >
{
  typedef composite_point meter_type;
  typedef std::shared_ptr<meter_type> meter_ptr;
  typedef std::map< std::string, composite_meter > meter_map;
  class outgoing_statistics;

public:
  virtual void configure() override;
  virtual void reconfigure() override;
  // ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;

private:
  meter_ptr request_meter_(std::string meter_name, size_t size);
  meter_ptr create_meter_(meter_map& meters, size_t size);
  meter_ptr notify_meter_(std::string meter_name, size_t size);
  meter_ptr other_meter_(size_t size);
  
private:
  bool _enable_write_size = false;
  bool _enable_error_stat = false;
  meter_map _req_meters;
  meter_map _ntf_meters;
  composite_meter  _other;
  std::mutex _mutex;
  std::shared_ptr<outgoing_statistics> _outgoing_statistics;
};

}}
