#include "../include/common.h"
#include "../include/text.h"
#include "../include/bnf.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

extern int create_pair_bnf(/*{{{*/
  const char*       pair_bnf_str
  , BNF*            bnf
  , PAIR_BNF*       pair_bnf
  , const int       pair_bnf_size
) {

  const int line_total = count_line_total(pair_bnf_str);
  assert(line_total < pair_bnf_size);

  pair_bnf[0].total_size = pair_bnf_size;
  pair_bnf[0].used_size = 0;

  for (int line=0; line<line_total; line++) {
    pair_bnf[line].id = line;
    pair_bnf[line].total_size = pair_bnf_size;
    pair_bnf[line].used_size = line_total;

    int begin, end, i;

    get_next_left_index(pair_bnf_str, line, &begin, &end);
    for (i=0; i<bnf[0].total_size; i++) {
      if (strlen(bnf[i].name) == end-begin) {
        if (is_same_word(bnf[i].name, 0, strlen(bnf[i].name), pair_bnf_str, begin, end)) {
          assert(is_lex(bnf[i]));
          pair_bnf[line].left_bnf_id = i;
          break;
        }
      }
    }
    assert(i < bnf[0].total_size);

    get_next_right_index(pair_bnf_str, line, &begin, &end);
    for (i=0; i<bnf[0].total_size; i++) {
      if (strlen(bnf[i].name) == end-begin) {
        if (is_same_word(bnf[i].name, 0, strlen(bnf[i].name), pair_bnf_str, begin, end)) {
          assert(is_lex(bnf[i]));
          pair_bnf[line].right_bnf_id = i;
          break;
        }
      }
    }
    assert(i < bnf[0].total_size);
  }

  return line_total;
}/*}}}*/
extern void print_pair_bnf(FILE *fp, const PAIR_BNF* pair_bnf, const BNF* bnf) {/*{{{*/
  for (int i=0; i<pair_bnf[0].used_size; i++) {
    fprintf(fp, "%d | %s %s   -   %s %s\n", i, bnf[pair_bnf[i].left_bnf_id].name, bnf[pair_bnf[i].left_bnf_id].def, bnf[pair_bnf[i].right_bnf_id].name, bnf[pair_bnf[i].right_bnf_id].def);
  }
}/*}}}*/
extern bool is_valid_paren_token(/*{{{*/
  const   int        token_begin_index
  , const int        token_end_index
  , const LEX_TOKEN* token
  , const PAIR_BNF*  pair_bnf
) {

  bool ret=true;

  for (int pair_index=0; pair_index<pair_bnf[0].used_size; pair_index++) {
    if (!ret) break;
    const int left  = pair_bnf[pair_index].left_bnf_id;
    const int right = pair_bnf[pair_index].right_bnf_id;

    int count = 0;
    for (int token_index=token_begin_index; token_index<token_end_index; token_index++) {
      if (!ret) break;
      if (token[token_index].kind == left) count++;
      if (token[token_index].kind == right) count--;
      if (count < 0) ret = false;
    }
    if (count != 0) ret = false;
  }

  return ret;
}/*}}}*/
