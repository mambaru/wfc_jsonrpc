add_library(
  matchmaker STATIC
  matchmaker.cpp
  matcher/array_match.cpp  
  matcher/builder.cpp  
  matcher/full_match.cpp  
  matcher/match_factory.cpp  
  matcher/object_match.cpp  
  matcher/prefix_match.cpp  
  matcher/regex_match.cpp  
  matcher/true_match.cpp  
  matcher/variant_match.cpp
)

