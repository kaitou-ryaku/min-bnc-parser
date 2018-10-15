#ifndef __MIN_BNC_PARSER_LEXER__
#define __MIN_BNC_PARSER_LEXER__

#include "../min-regex/include/min-regex.h"

typedef struct {
  int         id;
  int         kind;
  const char* src;
  int         bgn;
  int         end;
} LEX_TOKEN;

typedef struct {
  int   kind;
  char* name;
  char* bnf;
  int   bnf_bgn;
  int   bnf_end;
  MIN_REGEX_NODE* node;
  int   node_bgn;
  int   node_end;
} LEX_BNF;

void lexer(const char *match_str);

#endif
