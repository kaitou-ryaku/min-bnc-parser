#ifndef __MIN_BNC_PARSER_LEXER__
#define __MIN_BNC_PARSER_LEXER__

#include "common.h"
#include "../min-regex/include/min-regex.h"

int create_lexer(
  const char*       bnf_str
  , BNF*            bnf
  , char*           name
  , const int       name_max_size
  , char*           lex
  , const int       lex_max_size
  , char*           simple
  , const int       simple_max_size
  , MIN_REGEX_NODE* node
  , const int       node_max_size
);

int match_lexer(
  LEX_TOKEN*       token
  , const int      token_max_size
  , const BNF*     bnf
  , const char*    src_str
);

void print_token(
  FILE*        fp
  , BNF*       bnf
  , LEX_TOKEN* token
  , const int  token_size
);

#endif
