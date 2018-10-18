#include "../include/lexer.h"
#include <stdio.h>
#include <assert.h>

int main(void) {

  const char* bnf_str = ""
    "  number     : (0|(1|2|3|4|5|6|7|8|9)\\d*)\n"
    "  identifier : (\\l|\\u|_)(\\l|\\u|_|\\d)*\n"
    "  compare    : (==|<|>|<=|>=|!=)\n"
    "  plusminus  : (+|-)\n"
    "  starslash  : (\\*|/)\n"
    "  lparen     : \\(\n"
    "  rparen     : \\)\n"
  ;

  const char* src_str = "2==(15+20)*203-(42-0)/(0-7*A0b+b)";

  BNF            lex[246]; // char:255 - メタ文字:9
  char           name[5000];
  char           bnf[5000];
  char           simple[5000];
  MIN_REGEX_NODE node[5000];

  initialize_bnf(lex, sizeof(lex)/sizeof(BNF));

  const int lex_size = create_lexer(
    bnf_str
    , lex    , sizeof(lex)    / sizeof(BNF)
    , name   , sizeof(name)   / sizeof(char)
    , bnf    , sizeof(bnf)    / sizeof(char)
    , simple , sizeof(simple) / sizeof(char)
    , node   , sizeof(node)   / sizeof(MIN_REGEX_NODE)
  );

  LEX_TOKEN token[1000];
  const int token_max_size = 1000;

  const int token_size = match_lexer(token, token_max_size, lex, lex_size, src_str);

  print_token(stderr, lex, lex_size, token, token_size);
}
