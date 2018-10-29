#ifndef __MIN_BNC_PARSER_PAIR_BNF__
#define __MIN_BNC_PARSER_PAIR_BNF__

#include "bnf.h"
#include <stdio.h>

int create_pair_bnf(
  const char*       pair_bnf_str
  , BNF*            bnf
  , PAIR_BNF*       pair_bnf
  , const int       pair_bnf_size
);

void print_pair_bnf(FILE *fp, const PAIR_BNF* pair_bnf, const BNF* bnf);

bool is_valid_paren_token(
  const   int        token_begin_index
  , const int        token_end_index
  , const LEX_TOKEN* token
  , const PAIR_BNF*  pair_bnf
);

#endif
