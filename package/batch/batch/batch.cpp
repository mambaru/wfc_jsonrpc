#include "batch.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>
#include <wfc/jsonrpc/target.hpp>

namespace wfc{ namespace jsonrpc{ 

void batch::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler)
{
  if ( this->suspended() || d==nullptr )
    return domain_proxy::perform_io( std::move(d), io_id, handler );

  auto beg = d->begin();
  beg = json::parser::parse_space(beg, d->end(), nullptr );
  
  if ( beg==d->end() )
    return;
  
  if ( *beg != '[' )
    return domain_proxy::perform_io( std::move(d), io_id, handler );
  
  typedef json::array< std::vector< 
    json::pointer<
      data_ptr,
      json::raw_value<data_type> 
    >
  > > batch_json;
  typedef std::vector< data_ptr > batch_type;
  batch_type batch;
  batch.reserve( d->size()/128 );
  json::json_error e;
  batch_json::serializer()(batch, d->begin(), d->end(), &e );
  if ( e )
    return aux::send_error_raw( incoming_holder(), std::make_unique<parse_error>(), handler);
  if ( batch.empty() )
    return aux::send_error_raw( incoming_holder(), std::make_unique<invalid_request>(), handler);

  auto pcount = std::make_shared< std::atomic<size_t> >();
  
  auto pbatch_res = std::make_shared<batch_type>();
  pbatch_res->reserve( batch.size() );
  std::vector< std::pair<incoming_holder, outgoing_handler_t> > holder_handler_list;
  for ( auto& bd : batch )
  {
    incoming_holder holder( std::move(bd));
    json::json_error e;
    holder.parse(&e);
    if ( e )
    {
      aux::send_error_raw( std::move(holder), std::make_unique<invalid_request>(), [&pbatch_res](data_ptr de){
        pbatch_res->push_back(std::move(de));
      });
    }
    else if ( holder.is_notify() )
    {
      domain_proxy::perform_incoming( std::move(holder), io_id, [this](outgoing_holder oholder)
      {
        JSONRPC_LOG_WARNING(this->name() << " notify error: " << oholder.detach() )
      }
      );
    }
    else if ( holder.is_request() )
    {
      size_t pos = pbatch_res->size();
      pbatch_res->push_back(nullptr);
      holder_handler_list.push_back(
        std::make_pair(
          std::move(holder),
          [pos, pbatch_res, pcount, handler](outgoing_holder oholder)
          {
            (*pbatch_res)[pos] = oholder.detach();
            --(*pcount);
            if ( *pcount == 0 )
            {
              auto rd = std::make_unique<data_type>();
              rd->reserve( pbatch_res->size() * 128 );
              batch_json::serializer()(*pbatch_res, std::back_inserter(*rd) );
              handler( std::move(rd) );
            }
          }
        )
      );
    }
    else
    {
      aux::send_error_raw( std::move(holder), std::make_unique<invalid_request>(), [&pbatch_res](data_ptr de){
        pbatch_res->push_back(std::move(de));
      });
      
    }
  }
  
  if ( holder_handler_list.empty() )
  {
    if ( !pbatch_res->empty() )
    {
      auto rd = std::make_unique<data_type>();
      rd->reserve( pbatch_res->size() * 128 );
      batch_json::serializer()(*pbatch_res, std::back_inserter(*rd) );
      handler( std::move(rd) );
    }
    return;
  }

  *pcount = holder_handler_list.size();
  for ( auto& hh : holder_handler_list )
  {
    domain_proxy::perform_incoming( std::move(hh.first), io_id, std::move(hh.second) );
  }
}

}}
