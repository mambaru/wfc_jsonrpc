
#include "workflow_multiton.hpp"
#include "workflow.hpp"
#include "workflow_config_json.hpp"

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace jsonrpc{

namespace 
{ 
  WFC_NAME2(component_name, "jsonrpc-workflow") 

  class impl: public ::wfc::multiton<
    component_name,
    ::wfc::instance<workflow>,
    workflow_config_json
  >
  {};
}


workflow_multiton::workflow_multiton()
  : component( std::make_shared<impl>() )
{}

}}
