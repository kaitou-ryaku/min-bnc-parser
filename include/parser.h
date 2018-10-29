#ifndef __MIN_BNC_PARSER_PARSER__
#define __MIN_BNC_PARSER_PARSER__

#include "common.h"
#include "../min-regex/include/min-regex.h"

int parse_token_list(
  const   LEX_TOKEN* token
  , const BNF*       bnf
  , const PAIR_BNF*  pair_bnf
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
