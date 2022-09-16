#pragma once

#include "extract_config.hpp"
#include <wfc/jsonrpc/domain_proxy.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace jsonrpc{ 

class extract
  : public ::wfc::jsonrpc::domain_proxy< extract_config, ::wfc::nostat>
{
public:
  extract();
  virtual void configure() override;
  virtual void reconfigure() override;
  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) final override;
private:
  void perform_io_stream_(data_ptr d, io_id_t io_id, const output_handler_t& handler);
  void perform_io_pack_(data_ptr d, io_id_t io_id, const output_handler_t& handler);
private:
  typedef data_type::const_iterator const_iterator;
  const_iterator  perform_io_once_(const_iterator beg, const_iterator end, io_id_t io_id, const output_handler_t& handler);
  const_iterator find_first_(const_iterator beg, const_iterator end) const;
  bool parse_once_(const_iterator beg, const_iterator end, const_iterator* out) const;
private:
  std::atomic_bool _stream_mode;
  std::atomic_bool _show_parse_errors;
};

}}
