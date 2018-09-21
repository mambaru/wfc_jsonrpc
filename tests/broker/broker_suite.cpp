#include <fas/testing.hpp>
#include <wfc/json.hpp>
#include <matchmaker/matchmaker.hpp>
#include <utility>
#include <regex>
#include <boost/regex.hpp>

using namespace wfc::json::literals;
using namespace wfc::jsonrpc;
using json_error = wfc::json::json_error;
using strerr = wfc::json::strerror;

struct _matchmaker_;

namespace std
{
  std::ostream& operator << (std::ostream& os, const json_error& e)
  {
    os << wfc::json::strerror::message(e);
    return os;
  }
}

static const std::string configs[]=
{
  "'hello'"_json,                // 0
  "'hell'"_json,                 // 1
  "''hello''"_json,              // 2
  "'\"hell\"'"_json,             // 3
  "'.*hello.*'"_json,                 // 4
  "'.*hell.*'"_json,                  // 5
  "['.hello.', '.hell.', '.hellx.']"_json, // 6 По сути это проверка целого слова, т.к. нужно учитывать кавычки
  "{'foo':'bar'}"_json,                // 7
  "{'foo':'bar', 'baz':{'.*':'.(\\\\w+)(\\\\.|_)?(\\\\w*)@(\\\\w+)(\\\\.(\\\\w+))+.'}}"_json              // 9
  //"{'foo':'bar', 'baz':{'.*':'\\\"m.*'}}"_json              // 9
  
};

/*
static const std::string params[]=
{
  "'hello'"_json, 
  "'hell'"_json
};
*/
UNIT(create, "")
{
  using namespace fas::testing;
  GET_REF(_matchmaker_) = std::make_shared<matchmaker>();
}


template<bool Val, int mode, typename T>
void match(T& t, int nconfig, const std::string& json, std::string fl)
{
  using namespace fas::testing;
  json_error err;
  auto mm = GET_REF(_matchmaker_);
  t << message(configs[nconfig]) << " ws [" << json << "]";
  t << flush;
  bool res = mm->reconfigure( mode, configs[nconfig], err );
  t << is_false<assert>(err) << err << FAS_FL << std::endl << " from:" << fl;
  t << is_true<assert>(res) << " reconfigure for "<< configs[nconfig] << FAS_FL;
  t << stop;
  bool match_result = mm->match( json, err );
  t << is_false<assert>(err) << FAS_FL<< std::endl << " from:" << fl;
  t << stop;
  t << equal<expect, bool>(match_result, Val) << fl;
}

struct get_fl
{
  std::stringstream ss;
  template<typename V>
  get_fl& operator << (const V& val) {ss << val; return *this; }
  operator std::string () const { return ss.str();}
};

#define FAS_FLS std::string(get_fl() << FAS_FL)

UNIT(match0, "")
{
  using namespace fas::testing;
  t << nothing ;
  
  match<true,  match_mode::FullMatch>(t, 0, "'hello'"_json, FAS_FLS );
  match<false, match_mode::FullMatch>(t, 0, "'hell'"_json, FAS_FLS );
  match<false, match_mode::FullMatch>(t, 1, "'hello'"_json, FAS_FLS );
  match<true,  match_mode::FullMatch>(t, 1, "'hell'"_json, FAS_FLS );
  
  match<true,  match_mode::PrefixMatchValue>(t, 0, "'hello'"_json, FAS_FLS );
  match<false, match_mode::PrefixMatchValue>(t, 0, "'hell'"_json,  FAS_FLS );
  match<true,  match_mode::PrefixMatchValue>(t, 1, "'hello'"_json, FAS_FLS );
  match<true,  match_mode::PrefixMatchValue>(t, 1, "'hell'"_json,  FAS_FLS );

  match<false, match_mode::RegexMatchValue>(t, 0, "'hello'"_json, FAS_FLS );
  match<false, match_mode::RegexMatchValue>(t, 0, "'hell'"_json, FAS_FLS );
  match<false, match_mode::RegexMatchValue>(t, 1, "'hello'"_json, FAS_FLS );
  match<false, match_mode::RegexMatchValue>(t, 1, "'hell'"_json, FAS_FLS );

/*
  match<true,  match_mode::RegexMatchValue>(t, 2, "'hello'"_json, FAS_FLS );
  match<false, match_mode::RegexMatchValue>(t, 2, "'hell'"_json, FAS_FLS );
  match<false, match_mode::RegexMatchValue>(t, 3, "'hello'"_json, FAS_FLS );
  match<true,  match_mode::RegexMatchValue>(t, 3, "'hell'"_json, FAS_FLS );
*/

  match<true,  match_mode::RegexMatchValue>(t, 4, "'hello'"_json, FAS_FLS );
  match<false, match_mode::RegexMatchValue>(t, 4, "'hell'"_json, FAS_FLS );
  match<true,  match_mode::RegexMatchValue>(t,  5, "'hello'"_json, FAS_FLS );
  match<true,  match_mode::RegexMatchValue>(t, 5, "'hell'"_json, FAS_FLS );
  match<true,  match_mode::RegexMatchValue>(t, 5, "'hellx'"_json, FAS_FLS );
  
  match<true, match_mode::RegexMatchValue>(t, 6, "'hello'"_json, FAS_FLS );
  match<true, match_mode::RegexMatchValue>(t, 6, "'hell'"_json, FAS_FLS );
  match<true, match_mode::RegexMatchValue>(t, 6, "'hellx'"_json, FAS_FLS );
  
  match<true,  match_mode::FullMatch>(t, 7, "{'foo':'bar'}"_json, FAS_FLS );
  match<true,  match_mode::FullMatch>(t, 7, "{'foo1':'bar', 'foo':'bar', 'foo':'bar1'}"_json, FAS_FLS );
  match<false, match_mode::FullMatch>(t, 7, "{'foo1':'bar', 'foo':'bar1'}"_json, FAS_FLS );
  
  match<true,  match_mode::PrefixMatch>(t, 7, "{'foo':'bar'}"_json, FAS_FLS );
  match<true,  match_mode::PrefixMatch>(t, 7, "{'foo1':'bar', 'foo':'bar', 'foo':'bar1'}"_json, FAS_FLS );
  match<false, match_mode::PrefixMatch>(t, 7, "{'fo1':'bar', 'foo':'ba1'}"_json, FAS_FLS );
  
  
  /*boost::regex reg("\"(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+\"");
  bool res = boost::regex_match("\"migashko@gmail.com\"", reg);
  t << is_true<assert>(res) << FAS_FL;
  match<true,  match_mode::RegexMatch>(t, 8, "'migashko'"_json, FAS_FLS );
  match<true,  match_mode::RegexMatch>(t, 8, "'migashko@gmail.com'"_json, FAS_FLS );*/
  // TODO: regex для строк отдельной веткой 
  match<true,  match_mode::RegexMatch>(t, 8, "{'foo':'bar', 'baz':{'email':'migashko@gmail.com'}}"_json, FAS_FLS );
  
}

BEGIN_SUITE(broker_suite, "")
  ADD_VALUE(_matchmaker_, std::shared_ptr<matchmaker> )
  ADD_UNIT( create )
  ADD_UNIT( match0 )
  ADD_UNIT( match0 )
END_SUITE(broker_suite)
