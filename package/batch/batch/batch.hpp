#pragma once

#include "batch_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{ 

class batch
  : public ::wfc::jsonrpc::domain_proxy< batch_config, ::wfc::nostat>
{
public:
  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) final override;

};

}}
