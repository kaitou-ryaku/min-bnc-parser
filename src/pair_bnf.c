#include "../include/common.h"
#include "../include/text.h"
#include "../include/bnf.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

extern int create_pair_bnf(
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
}

extern void print_pair_bnf(FILE *fp, const PAIR_BNF* pair_bnf, const BNF* bnf) {
  for (int i=0; i<pair_bnf[0].used_size; i++) {
    fprintf(fp, "%d | %s %s   -   %s %s\n", i, bnf[pair_bnf[i].left_bnf_id].name, bnf[pair_bnf[i].left_bnf_id].def, bnf[pair_bnf[i].right_bnf_id].name, bnf[pair_bnf[i].right_bnf_id].def);
  }
}
