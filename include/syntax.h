#ifndef __MIN_BNC_PARSER_PARSER__
#define __MIN_BNC_PARSER_PARSER__

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

int parse_token_list(
  const   LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
  , const int        pt_max_size
  , bool*            memo
  , const int        memo_size
);

void all_parse_tree_to_dot(
  FILE*               fp
  , const int         pt_size
  , const PARSE_TREE* pt
  , const BNF*        bnf
  , const LEX_TOKEN*  token
  , const char*       fontsize
);

void origin_parse_tree_to_dot(
  FILE*               fp
  , const int         origin
  , const PARSE_TREE* pt
  , const BNF*        bnf
  , const LEX_TOKEN*  token
  , const char*       fontsize
  , const char*       meta_color
  , const char*       lex_color
  , const char*       syntax_color
);

#endif
