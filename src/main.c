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

  LEX_BNF        lex[5000];
  char           name[5000];
  char           bnf[5000];
  char           simple[5000];
  MIN_REGEX_NODE node[5000];

  const int lex_max_size    = 5000;
  const int name_max_size   = 5000;
  const int bnf_max_size    = 5000;
  const int simple_max_size = 5000;
  const int node_max_size   = 5000;

  const int lex_size = create_lexer(bnf_str, lex, lex_max_size, name, name_max_size, bnf, bnf_max_size, simple, simple_max_size, node, node_max_size);

  LEX_TOKEN token[1000];
  const int token_max_size = 1000;

  const int token_size = match_lexer(token, token_max_size, lex, lex_size, src_str);

  print_token(stderr, lex, lex_size, token, token_size);

  const char* syntax_str = ""
    "  EXPRESSION : FORMULA ( @ | `compare` FORMULA )\n";
    "  FORMULA    : ( @ | `plusminus` ) TERM ( `plusminus` | TERM )*\n"
    "  TERM       : FACTOR ( @ | `starslash` FACTOR )*\n";
    "  FACTOR     : ( IMMEDIATE | VARIABLE | `lparen` EXPRESSION `rparen` )\n";
    "  IMMEDIATE  : `number`\n";
    "  VARIABLE   : `identify`\n";
  ;
}
