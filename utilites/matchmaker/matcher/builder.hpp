#pragma once

#include "ibuilder.hpp"
#include "match_factory.hpp"

namespace wfc{ namespace jsonrpc{ 

class builder
  : public ibuilder
  , public std::enable_shared_from_this<builder>
{
public:
  explicit builder(int mode);
  virtual std::shared_ptr<imatcher> build_name() override;
  virtual std::shared_ptr<imatcher> build_value(const char* beg, const char* end, json::json_error& err) override;
  virtual std::shared_ptr<imatcher> build_value_nv(const char* beg, const char* end, json::json_error& err) override;
private:
  std::shared_ptr<imatcher> build_value_(const char* beg, const char* end, json::json_error& err, bool nv);
private:
  match_factory _factory;
};

}}
