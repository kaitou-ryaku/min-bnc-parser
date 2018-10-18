#ifndef __MIN_BNC_PARSER_BNF__
#define __MIN_BNC_PARSER_BNF__

#include "common.h"
#include "../min-regex/include/min-regex.h"

void initialize_bnf(BNF* bnf, const int bnf_max_size);

int read_bnf(
  const char*       syntax_str
  , BNF*            bnf
  , const int       bnf_max_size
  , char*           name
  , const int       name_max_size
  , char*           def
  , const int       def_max_size
);

#endif
