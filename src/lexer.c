#include "../include/lexer.h"
#include "../min-regex/include/min-regex.h"
#include <stdio.h>
#include <string.h>

extern void lexer(const char *match_str) {
  fprintf(stderr, "LEXICAL ANALYSIS OF\n%s\n", match_str);
  // char simple_regex[200];
  // simplify_regex(regex_str, simple_regex, 200);
  // MIN_REGEX_NODE node[100];
  // regex_to_all_node(simple_regex, node, 100);
  // MIN_REGEX_MATCH match[50];
  // // const int step = exact_match(match_str, node, match, 50);
  // const int index = forward_shortest_match( match_str, node, match, 50);

  // for (int i=0; i<index; i++) fprintf(stderr, "%c", match_str[i]);
  // fprintf(stderr, "\n");

  LEX_BNF lex[6];

  MIN_REGEX_NODE number[300];
  lex[0].node = number;
  lex[0].name = "number";
  lex[0].bnf  = "(0|(1|2)((0|1)|2)*)";

  MIN_REGEX_NODE identity[300];
  lex[1].node = identity;
  lex[1].name = "identity";
  lex[1].bnf  = "(a|b)((0|a)|b)*";

  MIN_REGEX_NODE plusminus[300];
  lex[2].node = plusminus;
  lex[2].name = "plusminus";
  lex[2].bnf  = "(+|-)";

  MIN_REGEX_NODE starslash[300];
  lex[3].node = starslash;
  lex[3].name = "starslash";
  lex[3].bnf  = "(\\*|/)";

  MIN_REGEX_NODE lparen[300];
  lex[4].node = lparen;
  lex[4].name = "lparen";
  lex[4].bnf  = "\\(";

  MIN_REGEX_NODE rparen[300];
  lex[5].node = rparen;
  lex[5].name = "rparen";
  lex[5].bnf  = "\\)";

  for (int kind=0; kind<6; kind++) {
    lex[kind].kind    = kind;
    lex[kind].bnf_bgn = 0;
    lex[kind].bnf_end = strlen(lex[kind].bnf);
    regex_to_all_node(lex[kind].bnf, lex[kind].node, 300);
  }

  LEX_TOKEN token[100];
  int token_id = 0;
  int index = 0;
  while (index < strlen(match_str)) {
    const char *rest = &(match_str[index]);

    for (int kind=0; kind<6; kind++) {
      MIN_REGEX_MATCH match[200];
      int delta = forward_longest_match( rest, lex[kind].node, match, 200);
      if (delta > 0) {
        token[token_id].id   = token_id;
        token[token_id].kind = kind;
        token[token_id].src  = match_str;
        token[token_id].bgn  = index;
        token[token_id].end  = index+delta;

        for (int j=0; j<index; j++) fprintf(stderr, " ");
        for (int j=0; j<delta; j++) fprintf(stderr, "%c", token[token_id].src[index+j]);
        for (int j=0; j<strlen(match_str)-index-delta; j++) fprintf(stderr, " ");
        fprintf(stderr, " | %s\n", lex[kind].name);

        index = index + delta;
        token_id++;
        break;
      }
    }
  }
}
