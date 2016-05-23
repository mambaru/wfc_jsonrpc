#include "broker.hpp"
#include "broker_impl.hpp"
#include <wfc/logger.hpp>

namespace wfc{

class broker::impl: public broker_impl
{
public:
  impl()
  {}
};

broker::~broker()
{
}

broker::broker()
{
}

void broker::reconfigure()
{
  if ( auto g = this->global() )
  {
    if ( _impl == nullptr )
    {
       _impl = std::make_shared<broker::impl>( );
    }
    _impl->reconfigure(this->options(), this->global()->registry);
  }
}

void broker::stop(const std::string&) 
{
}

void broker::start(const std::string&)
{
}

void broker::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  _impl->reg_io(io_id, itf);
}

void broker::unreg_io(io_id_t io_id)
{
  _impl->unreg_io(io_id);
}

void broker::perform_io(data_ptr d, io_id_t io_id, io_outgoing_handler_t handler) 
{
  _impl->perform_io(std::move(d), io_id, std::move(handler) );
}

void broker::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  _impl->perform_incoming(std::move(holder), io_id, std::move(handler) );
}
  
void broker::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  _impl->perform_outgoing(std::move(holder), io_id );
}

void broker::generate1(broker_config& opt, const std::string& /*type*/)
{
  opt = broker_config();
  opt.target="*default-target*";
  opt.reject={"*reject", "method", "list*"};
  opt.targets.push_back(broker_config::method_target());
  opt.targets.back().target="*target for method list*";
  opt.targets.back().methods={"*method", "list*"};
}


}
