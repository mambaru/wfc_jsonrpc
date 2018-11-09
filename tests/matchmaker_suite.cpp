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

namespace {

struct _matchmaker_;
static const std::string configs[]=
{
  "'hello'"_json,                // 0
  "'hell'"_json,                 // 1
  "'hello'"_json,              // 2
  "'hell'"_json,             // 3
  "'.*hello.*'"_json,            // 4
  "'.*hell.*'"_json,             // 5
  "['hello', 'hell', 'hellx']"_json, // 6 По сути это проверка целого слова, т.к. нужно учитывать кавычки
  "{'foo':'bar'}"_json,          // 7
  "{'foo':'bar', 'baz':{'.*':'.(\\\\w+)(\\\\.|_)?(\\\\w*)@(\\\\w+)(\\\\.(\\\\w+))+.'}}"_json,              // 8
  "{'foo':'bar', 'baz':{'email':null}}"_json,              // 9
  "{'foo':'bar', 'baz':{'a':['b','c','d']}}"_json,              // 10
  "{'foo':'bar', 'baz':{'a':[['b','c','d']]}}"_json,          // 11
  "{'foo':'bar', 'baz':{'a':[['b',[['c']],'d']]}}"_json,      // 12
  "{'foo':'bar', 'baz':{'a':'.{13,17}'}}"_json,       // 13
  "{'baz':{'email':'.(\\\\w+)(\\\\.|_)?(\\\\w*)@(\\\\w+)(\\\\.(\\\\w+))+.', 'email':'.{17,19}'}, 'foo':'bar'}"_json, // 14
  "{'baz':{'email':'.(\\\\w+)(\\\\.|_)?(\\\\w*)@(\\\\w+)(\\\\.(\\\\w+))+.', 'email':'.{17,19}', 'user':'migashko'}, 'foo':'bar'}"_json // 15
  //"{'baz':{'email':'.{0,190}'}, 'foo':'bar'}"_json // 14
};

UNIT(create, "")
{
  using namespace fas::testing;
  GET_REF(_matchmaker_) = std::make_shared<matchmaker>();
}


template<bool Val, int mode, typename T>
void match(T& t, int nconfig, const std::string& json, const std::string& fl)
{
  using namespace fas::testing;
  json_error err;
  auto mm = GET_REF(_matchmaker_);
  t << message(configs[nconfig]) << " ws " << json;
  t << flush;
  bool res = mm->reconfigure( mode, configs[nconfig], err );
  t << is_false<assert>(err) << wfc::json::strerror::message_trace(err, json.begin(), json.end() )
    << FAS_FL << std::endl << " from:" << fl;
  t << is_true<assert>(res) << " reconfigure for "<< configs[nconfig] << FAS_FL;
  t << stop;
  bool match_result = mm->match( json, err );
  t << is_false<assert>(err) << wfc::json::strerror::message_trace(err, json.begin(), json.end() ) 
    << FAS_FL<< std::endl << " from:" << fl;
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

  match<true, match_mode::RegexMatchValue>(t, 0, "'hello'"_json, FAS_FLS );
  match<false, match_mode::RegexMatchValue>(t, 0, "'hell'"_json, FAS_FLS );
  match<false, match_mode::RegexMatchValue>(t, 1, "'hello'"_json, FAS_FLS );
  match<true, match_mode::RegexMatchValue>(t, 1, "'hell'"_json, FAS_FLS );

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
  match<true,  match_mode::FullMatch>(t, 7, "{'foo1':'bar', 'foo':'bar', 'foo2':'bar1'}"_json, FAS_FLS );
  match<false, match_mode::FullMatch>(t, 7, "{'foo1':'bar', 'foo':'bar1'}"_json, FAS_FLS );
  
  match<true,  match_mode::PrefixMatch>(t, 7, "{'foo':'bar'}"_json, FAS_FLS );
  match<true,  match_mode::PrefixMatch>(t, 7, "{'foo1':'bar', 'foo':'bar', 'foo2':'bar1'}"_json, FAS_FLS );
  match<false, match_mode::PrefixMatch>(t, 7, "{'fo1':'bar', 'foo':'ba1'}"_json, FAS_FLS );
  
  match<true,  match_mode::RegexMatch>(t, 8, "{'foo':'bar', 'baz':{'email':'migashko@gmail.com'}}"_json, FAS_FLS );
  
  match<false, match_mode::RegexMatch>(t, 9, "{'foo':'bar', 'baz':{'email1':null}}"_json, FAS_FLS );
  match<true,  match_mode::RegexMatch>(t, 9, "{'foo':'bar', 'baz':{'email':null}}"_json, FAS_FLS );
  match<true,  match_mode::RegexMatch>(t, 9, "{'foo':'bar', 'baz':{'email':'anystring'}}"_json, FAS_FLS );
  
  match<true,  match_mode::FullMatch>(t, 10, "{'foo':'bar', 'baz':{'a':'b'}}"_json, FAS_FLS );
  match<true,  match_mode::FullMatch>(t, 10, "{'foo':'bar', 'baz':{'a':'c'}}"_json, FAS_FLS );
  match<true,  match_mode::FullMatch>(t, 10, "{'foo':'bar', 'baz':{'a':'d'}}"_json, FAS_FLS );
  match<false, match_mode::FullMatch>(t, 10, "{'foo':'bar', 'baz':{'a':'e'}}"_json, FAS_FLS );
  
  match<true, match_mode::FullMatch>(t, 11, "{'foo':'bar', 'baz':{'a':['b','d','c']}}"_json, FAS_FLS );
  match<true, match_mode::FullMatch>(t, 11, "{'foo':'bar', 'baz':{'a':['b','d','c', 1]}}"_json, FAS_FLS );
  match<false, match_mode::FullMatch>(t, 11, "{'foo':'bar', 'baz':{'a':['b','d']}}"_json, FAS_FLS );
  
  match<true, match_mode::FullMatch>(t, 12, "{'foo':'bar', 'baz':{'a':['b','d',['c']]}}"_json, FAS_FLS );
  match<true, match_mode::FullMatch>(t, 12, "{'foo':'bar', 'baz':{'a':[['c', 'e'],'b','d']}}"_json, FAS_FLS );
  match<false, match_mode::FullMatch>(t, 12, "{'foo':'bar', 'baz':{'a':[['c', 'e'],'b',]}}"_json, FAS_FLS );
  match<false, match_mode::FullMatch>(t, 12, "{'foo':'bar', 'baz':{'a':[['e'],'b','d']}}"_json, FAS_FLS );
  
  constexpr int mode1 = match_mode::RegexMatchValue | match_mode::FullMatchName;
  match<true, mode1>(t, 13, "{'foo':'bar', 'baz':{'a':['b','d','c']}}"_json, FAS_FLS );
  match<true, mode1>(t, 13, "{'foo':'bar', 'baz':{'a':['b','d','c','d']}}"_json, FAS_FLS );
  match<false, mode1>(t, 13, "{'foo':'bar', 'baz':{'a':['b','d']}}"_json, FAS_FLS );
  match<false, mode1>(t, 13, "{'foo':'bar', 'baz':{'a':['b','d','c','d','e']}}"_json, FAS_FLS );
  
  match<true, mode1>(t, 14, "{'foo':'bar','baz':{'email':'migashko@gmail.com'}}"_json, FAS_FLS );
  match<false,mode1>(t, 14, "{'foo1':'bar', 'baz':{'email':'migashko@gmail.com'}}"_json, FAS_FLS );
  match<false,mode1>(t, 14, "{'foo':'bar1', 'baz':{'email':'migashko@gmail.com'}}"_json, FAS_FLS );
  match<false,mode1>(t, 14, "{'foo':'bar', 'baz':{'email':'mig@gmail.com'}}"_json, FAS_FLS );
  match<false,mode1>(t, 14, "{'foo':'bar', 'baz':{'email':'migashko1111@gmail.com'}}"_json, FAS_FLS );
  match<false, mode1>(t, 14, "{'foo':'bar', 'baz':{'emailX':'migashko@gmail.com'}}"_json, FAS_FLS );
  
  match<false, mode1>(t, 15, "{'foo':'bar', 'baz':{'emailX':'migashko@gmail.com', 'user':'migashko'}}"_json, FAS_FLS );
  match<true, mode1>(t, 15, "{'foo':'bar', 'baz':{'email':'migashko@gmail.com', 'user':'migashko'}}"_json, FAS_FLS );
  match<false, mode1>(t, 15, "{'foo':'bar', 'baz':{'email':'migashko@gmail.com', 'user':'migashko1'}}"_json, FAS_FLS );
  match<true, mode1>(t, 15, "{'foo':'bar', 'baz':{'email':'migashko1@gmail.com', 'user':'migashko'}}"_json, FAS_FLS );
  match<false, mode1>(t, 15, "{'foo':'bar', 'baz':{'email':'migashko1111@gmail.com', 'user':'migashko'}}"_json, FAS_FLS );
}

} // namespace

BEGIN_SUITE(matchmaker_suite, "")
  ADD_VALUE(_matchmaker_, std::shared_ptr<matchmaker> )
  ADD_UNIT( create )
  ADD_UNIT( match0 )
  ADD_UNIT( match0 )
END_SUITE(matchmaker_suite)
