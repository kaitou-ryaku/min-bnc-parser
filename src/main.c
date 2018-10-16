#include "../include/lexer.h"
#include <stdio.h>
#include <assert.h>

int main(int argc, const char **argv) {
  if (argc != 1) {
    fprintf(stderr, "Invalid argc (%d)\n", argc);
    return 1;
  }

  const char* lex_bnf = ""
    "  number     : (0|(1|2)((0|1)|2)*)\n"
    "  identifier : (a|b)((0|a)|b)*\n"
    "  plusminus  : (+|-)\n"
    "  starslash  : (\\*|/)\n"
    "  lparen     : \\(\n"
    "  rparen     : \\)\n"
  ;

  const char* src = "(11+20)*201-(21-1)/(0-1*a0b+b)";

  lexer(lex_bnf, src);
}
