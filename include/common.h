#ifndef __MIN_BNC_PARSER_COMMON__
#define __MIN_BNC_PARSER_COMMON__

#include "../min-regex/include/min-regex.h"

typedef struct {
  int             kind;
  const char*     str;
  int             name_begin;
  int             name_end;
  int             bnf_begin;
  int             bnf_end;
  const char*     name;
  const char*     bnf;
  const char*     simple;
  MIN_REGEX_NODE* node;
  int             node_begin;
  int             node_end;
} BNF;

#endif
