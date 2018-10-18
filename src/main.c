#include "../include/bnf.h"
#include "../include/lexer.h"
#include "../include/parser.h"
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

  BNF bnf[246]; // char:255 - メタ文字:9
  initialize_bnf(bnf, sizeof(bnf)/sizeof(BNF));

  char           name[5000];
  char           def[5000];
  char           simple[5000];
  MIN_REGEX_NODE node[5000];

  const int bnf_size = create_lexer(
    lex_str
    , bnf    , sizeof(bnf)    / sizeof(BNF)
    , name   , sizeof(name)   / sizeof(char)
    , def    , sizeof(def)    / sizeof(char)
    , simple , sizeof(simple) / sizeof(char)
    , node   , sizeof(node)   / sizeof(MIN_REGEX_NODE)
  );

  const char* syntax_str = ""
    "  EXPRESSION : FORMULA ( @ | `compare` FORMULA )\n";
    "  FORMULA    : ( @ | `plusminus` ) TERM ( `plusminus` | TERM )*\n"
    "  TERM       : FACTOR ( @ | `starslash` FACTOR )*\n";
    "  FACTOR     : ( IMMEDIATE | VARIABLE | `lparen` EXPRESSION `rparen` )\n";
    "  IMMEDIATE  : `number`\n";
    "  VARIABLE   : `identify`\n";

  LEX_TOKEN token[1000];

  const char* src_str = "2==(15+20)*203-(42-0)/(0-7*A0b+b)";
  const int token_size = match_lexer(token, sizeof(token)/sizeof(LEX_TOKEN), bnf, bnf_size, src_str);

  print_token(stderr, bnf, token, token_size);
}
