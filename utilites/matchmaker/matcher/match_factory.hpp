#pragma once

#include <memory>

namespace wfc{ namespace jsonrpc{ 

struct imatcher;

class match_factory
{
public:
  match_factory(int mode);
  std::shared_ptr<imatcher> create();
private:
  int _mode;
};

}}
