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
  int          kind;
  const char* str;
  int         name_begin;
  int         name_end;
  int         bnf_begin;
  int         bnf_end;
  MIN_REGEX_NODE* node;
  int         node_begin;
  int         node_end;
} LEX_BNF;

void lexer(const char* lex_bnf, const char *src_str);

#endif
