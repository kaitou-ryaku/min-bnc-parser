#ifndef __MIN_BNC_PARSER_PARSER__
#define __MIN_BNC_PARSER_PARSER__

#include "common.h"
#include "../min-regex/include/min-regex.h"

extern int create_parser(
  const char*       syntax_str
  , const int       lex_size
  , char*           work
  , const int       work_max_size
  , BNF*            bnf
  , const int       bnf_max_size
  , char*           name
  , const int       name_max_size
  , char*           def
  , const int       def_max_size
  , char*           simple
  , const int       simple_max_size
  , MIN_REGEX_NODE* node
  , const int       node_max_size
);

#endif
