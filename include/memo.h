#ifndef __MIN_BNC_PARSER_MEMO__
#define __MIN_BNC_PARSER_MEMO__

#include <stdbool.h>
#include "common.h"

void initialize_memo(PARSE_MEMO* memo, const int memo_max_size);
int register_memo(const bool state, const char bnf_id, const int token_begin_index, const int token_end_index, PARSE_MEMO* memo);
bool search_memo(const char bnf_id, const int token_begin_index, const int token_end_index, const PARSE_MEMO* memo);

#endif
