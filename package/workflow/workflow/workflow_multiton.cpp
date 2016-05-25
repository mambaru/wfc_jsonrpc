
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
  WFC_NAME2(multiton_name, "jsonrpc-queue") 

  class impl: public ::wfc::multiton<
    multiton_name,
    ::wfc::instance<workflow>,
    workflow_config_json,
    ::wfc::component_features::CommonWorkflow
  >
  {};
}


workflow_multiton::workflow_multiton()
  : component( std::make_shared<impl>() )
{}

}}
