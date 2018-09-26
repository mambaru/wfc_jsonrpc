#pragma once

#include "ibuilder.hpp"
#include "match_factory.hpp"

namespace wfc{ namespace jsonrpc{ 

class builder
  : public ibuilder
  , public std::enable_shared_from_this<builder>
{
public:
  builder(int mode);
  virtual std::shared_ptr<imatcher> build(const char* beg, const char* end, json::json_error& err) override;
  virtual std::shared_ptr<imatcher> build_nv(const char* beg, const char* end, json::json_error& err) override;
private:
  std::shared_ptr<imatcher> build_(const char* beg, const char* end, json::json_error& err, bool nv);
private:
  match_factory _factory;
};

}}
