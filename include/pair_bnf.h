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

#endif
