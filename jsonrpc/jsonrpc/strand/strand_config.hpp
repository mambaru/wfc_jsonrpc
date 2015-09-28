#pragma once

#include <iow/io/strand/mtoptions.hpp>

namespace wfc{

struct strand_config
{
  typedef ::iow::io::strand::mtoptions strand_options;
  std::string target;
  strand_options incoming;
  strand_options outgoing;
};

}
