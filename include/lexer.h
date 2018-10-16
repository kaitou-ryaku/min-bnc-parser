#ifndef __MIN_BNC_PARSER_LEXER__
#define __MIN_BNC_PARSER_LEXER__

#include "../min-regex/include/min-regex.h"

typedef struct {
  int         id;
  int         kind;
  const char* src;
  int         begin;
  int         end;
} LEX_TOKEN;

typedef struct {
  int         kind;
  const char* str;
  int         name_begin;
  int         name_end;
  int         bnf_begin;
  int         bnf_end;
  const char* name;
  const char* bnf;
  const char* simple;
  MIN_REGEX_NODE* node;
  int         node_begin;
  int         node_end;
} LEX_BNF;

int create_lexer(
  const char*       bnf_str
  , LEX_BNF*        lex
  , const int       lex_max_size
  , char*           name
  , const int       name_max_size
  , char*           bnf
  , const int       bnf_max_size
  , char*           simple
  , const int       simple_max_size
  , MIN_REGEX_NODE* node
  , const int       node_max_size
);

int match_lexer(
  LEX_TOKEN*       token
  , const int      token_max_size
  , const LEX_BNF* lex
  , const int      lex_size
  , const char*    src_str
);

void print_token(
  FILE*        fp
  , LEX_BNF*   lex
  , const int  lex_size
  , LEX_TOKEN* token
  , const int  token_size
);

#endif
