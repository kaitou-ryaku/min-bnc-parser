#ifndef __MIN_BNC_PARSER_LEXER__
#define __MIN_BNC_PARSER_LEXER__

#include "common.h"
#include "../min-regex/include/min-regex.h"

int create_lex(
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
  LEX_TOKEN*    token
  , const int   token_max_size
  , char*       name_array
  , const int   name_array_max_size
  , const BNF*  bnf
  , const char* src_str
);

void print_token_name (
  FILE*       fp
  , LEX_TOKEN t
);

void print_token_long_line(
  FILE*        fp
  , BNF*       bnf
  , LEX_TOKEN* token
  , const int  token_size
);

void print_token(
  FILE*        fp
  , BNF*       bnf
  , LEX_TOKEN* token
  , const int  token_size
);

bool is_token_kind(
  const char* name
  , const LEX_TOKEN token
  , const BNF* bnf
);

#endif
