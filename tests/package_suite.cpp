#include <fas/testing.hpp>
#include <package/jsonrpc_package.hpp>
#include <wfc/module/imodule.hpp>
#include <wfc/core/wfcglobal.hpp>
#include <wfc/asio.hpp>
#include <memory>

namespace {

struct _package_;
struct _global_;
struct _io_service_;

UNIT(init, "")
{
  using namespace fas::testing;
  auto pkg = std::make_shared<wfc::jsonrpc_package>();
  auto global = std::make_shared<wfc::wfcglobal>( GET_REF(_io_service_) );
  GET_REF(_package_) = pkg;
  GET_REF(_global_) = global;
  pkg->create(global);
  t << nothing;
}

UNIT(backlog, "")
{
  using namespace fas::testing;
  std::shared_ptr<wfc::wfcglobal> g = GET_REF(_global_);
  t << is_true<assert>(g!=nullptr) << FAS_FL;
  auto m = g->registry.template get_object<wfc::imodule>("module","jsonrpc-backlog", false);
  t << is_true<assert>(m!=nullptr) << FAS_FL;
}

UNIT(deinit, "")
{
  using namespace fas::testing;
  auto global = GET_REF(_global_);
  global->clear();
  t << nothing;
}

}
  
BEGIN_SUITE(package_suite, "")
  ADD_VALUE(_package_, std::shared_ptr<wfc::jsonrpc_package> )
  ADD_VALUE(_io_service_, wfc::asio::io_service )
  ADD_VALUE(_global_, std::shared_ptr<wfc::wfcglobal>)
  ADD_UNIT( init )
  ADD_UNIT( backlog )
  ADD_UNIT( deinit )
END_SUITE(package_suite)
