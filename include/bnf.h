#ifndef __MIN_BNC_PARSER_BNF__
#define __MIN_BNC_PARSER_BNF__

#include "common.h"
#include "../min-regex/include/min-regex.h"

void initialize_bnf(BNF* bnf, const int bnf_max_size);

int read_bnf(
  const char* syntax_str
  , BNF*      bnf
  , char*     name
  , const int name_max_size
  , char*     def
  , const int def_max_size
  , const int meta_lex_syntax_state
);

int search_bnf_next_lex( const int current_bnf_index, const BNF* bnf);
int search_bnf_next_syntax( const int current_bnf_index, const BNF* bnf);
int search_bnf_next_lex_or_syntax(const int current_bnf_index, const BNF* bnf);
bool is_meta(const BNF bnf);
bool is_lex(const BNF bnf);
bool is_syntax(const BNF bnf);
BNF node_to_bnf(const MIN_REGEX_NODE node, const BNF* bnf);
int node_to_bnf_id(const MIN_REGEX_NODE node, const BNF* bnf);
void print_bnf_unit(FILE* fp, const BNF bnf);

#endif
