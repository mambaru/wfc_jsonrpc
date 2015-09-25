#pragma once

#include <string>

namespace wfc{
  
struct strand_options
{
  bool disabled = false;
  size_t maxsize = 1024*1024;
  size_t wrnsize = 1024;
  size_t strands = 1;
  size_t threads = 1;
};

}
