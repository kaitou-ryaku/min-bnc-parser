#ifndef __MIN_BNC_PARSER_SYNTAX__
#define __MIN_BNC_PARSER_SYNTAX__

#include "common.h"
#include "../min-regex/include/min-regex.h"

extern int create_syntax(
  const char*       syntax_str
  , BNF*            bnf
  , char*           work
  , const int       work_max_size
  , char*           name
  , const int       name_max_size
  , char*           def
  , const int       def_max_size
  , char*           simple
  , const int       simple_max_size
  , MIN_REGEX_NODE* node
  , const int       node_max_size
);

extern void syntax_to_dot(
  FILE*        fp
  , BNF*       bnf
  , const char* fontsize
  , const char* width
  , const char* topic_color
  , const char* boundary_color
  , const char* normal_color
);

#endif
