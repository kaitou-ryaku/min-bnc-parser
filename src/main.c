#include "../include/bnf.h"
#include "../include/lexer.h"
#include <stdio.h>
#include <assert.h>

int main(void) {

  const char* lex_str = ""
    "  number     : (0|(1|2|3|4|5|6|7|8|9)\\d*)\n"
    "  identifier : (\\l|\\u|_)(\\l|\\u|_|\\d)*\n"
    "  compare    : (==|<|>|<=|>=|!=)\n"
    "  plusminus  : (+|-)\n"
    "  starslash  : (\\*|/)\n"
    "  lparen     : \\(\n"
    "  rparen     : \\)\n"
  ;

  const char* src_str = "2==(15+20)*203-(42-0)/(0-7*A0b+b)";

  BNF            bnf[246]; // char:255 - メタ文字:9
  char           name[5000];
  char           def[5000];
  char           simple[5000];
  MIN_REGEX_NODE node[5000];

  initialize_bnf(bnf, sizeof(bnf)/sizeof(BNF));

  const int bnf_size = create_lexer(
    lex_str
    , bnf    , sizeof(bnf)    / sizeof(BNF)
    , name   , sizeof(name)   / sizeof(char)
    , def    , sizeof(def)    / sizeof(char)
    , simple , sizeof(simple) / sizeof(char)
    , node   , sizeof(node)   / sizeof(MIN_REGEX_NODE)
  );

  LEX_TOKEN token[1000];
  const int token_max_size = 1000;

  const int token_size = match_lexer(token, token_max_size, bnf, bnf_size, src_str);

  print_token(stderr, bnf, token, token_size);
}
