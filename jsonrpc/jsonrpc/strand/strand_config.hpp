#pragma once

#include <string>
#include "strand_options.hpp"

namespace wfc{
 
struct strand_config
{
  std::string target;
  strand_options incoming;
  strand_options outgoing;
};

}
