#include "extract.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

namespace wfc{ namespace jsonrpc{ 

extract::extract()
  : _stream_mode()
  , _show_parse_errors()
{

}
void extract::configure()
{
  _stream_mode = this->options().stream_mode;
  _show_parse_errors = this->options().show_parse_errors;
}

void extract::reconfigure()
{
  _stream_mode = this->options().stream_mode;
  _show_parse_errors = this->options().show_parse_errors;
}

void extract::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler)
{
  if ( this->suspended() || d==nullptr )
    return domain_proxy::perform_io( std::move(d), io_id, handler );

  if ( !_stream_mode )
  {
    perform_io_pack_( std::move(d), io_id, handler);
  }
  else
  {
    perform_io_stream_( std::move(d), io_id, handler);
  }
}

void extract::perform_io_stream_(data_ptr , io_id_t, const output_handler_t&)
{
  // TODO: map<io_id, deque<char> >
  JSONRPC_LOG_FATAL("EXTRACT: stream_mode==true not implementation!");
}

void extract::perform_io_pack_(data_ptr d, io_id_t io_id, const output_handler_t& handler)
{
  this->perform_io_once_(d->begin(), d->end(), io_id, handler);
}


extract::const_iterator extract::perform_io_once_(const_iterator beg, const_iterator end, io_id_t io_id, const output_handler_t& handler)
{
  const_iterator fin = end;
  while ( beg!=end )
  {
    beg = find_first_(beg, end);
    if ( beg == end )
      return fin;

    if ( parse_once_(beg, end, &fin) )
    {
      domain_proxy::perform_io(std::make_unique<data_type>(beg, fin), io_id, handler);
      beg = fin;
    }
    else
      ++beg;
  }
  return fin;
}

extract::const_iterator extract::find_first_(const_iterator beg, const_iterator end) const
{
  for (;beg!=end; ++beg)
  {
    if ( *beg == '{' || *beg=='[' )
      break;
  }
  return beg;
}

bool extract::parse_once_(const_iterator beg, const_iterator end, const_iterator* out) const
{
  if ( beg == end )
    return false;

  const_iterator start = beg;
  wjson::json_error je;
  if ( *beg == '[' )
    beg = wjson::parser::parse_array(beg, end, &je);
  else if ( *beg == '{' )
    beg = wjson::parser::parse_object(beg, end, &je);
  else
    beg = wjson::parser::parse_value(beg, end, &je);

  if ( !je )
  {
    *out = beg;
  }
  else if ( _show_parse_errors)
  {
    JSONRPC_LOG_WARNING("EXTRACT parser error:" << wjson::strerror::message_trace(je, start, end) );
  }
  return je;
}

}}
