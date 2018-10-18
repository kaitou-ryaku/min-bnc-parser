#include "../include/bnf.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include <stdio.h>
#include <assert.h>

int main(void) {

  BNF bnf[246]; // char:255 - メタ文字:9
  initialize_bnf(bnf, sizeof(bnf)/sizeof(BNF));

  const char* lex_str = ""
    "  number     : (0|(1|2|3|4|5|6|7|8|9)\\d*)\n"
    "  identifier : (\\l|\\u|_)(\\l|\\u|_|\\d)*\n"
    "  compare    : (==|<|>|<=|>=|!=)\n"
    "  plusminus  : (+|-)\n"
    "  starslash  : (\\*|/)\n"
    "  lparen     : \\(\n"
    "  rparen     : \\)\n"
  ;

  char           lex_name[5000];
  char           lex_def[5000];
  char           lex_simple[5000];
  MIN_REGEX_NODE lex_node[5000];

  const int lex_size = create_lexer(
    lex_str
    , bnf        , sizeof(bnf)        / sizeof(BNF)
    , lex_name   , sizeof(lex_name)   / sizeof(char)
    , lex_def    , sizeof(lex_def)    / sizeof(char)
    , lex_simple , sizeof(lex_simple) / sizeof(char)
    , lex_node   , sizeof(lex_node)   / sizeof(MIN_REGEX_NODE)
  );

  const char* syntax_str = ""
    "  EXPRESSION : FORMULA ( @ | compare FORMULA )\n"
    "  FORMULA    : ( @ | plusminus ) TERM ( plusminus | TERM )*\n"
    "  TERM       : FACTOR ( @ | starslash FACTOR )*\n"
    "  FACTOR     : ( IMMEDIATE | VARIABLE | lparen EXPRESSION rparen )\n"
    "  IMMEDIATE  : number\n"
    "  VARIABLE   : identifier\n"
  ;

  char           work[1000];
  char           syntax_name[5000];
  char           syntax_def[5000];
  char           syntax_simple[5000];
  MIN_REGEX_NODE syntax_node[5000];

  const int parser_size = create_parser(
    syntax_str, lex_size
    , work          , sizeof(work)          / sizeof(work)
    , bnf           , sizeof(bnf)           / sizeof(BNF)
    , syntax_name   , sizeof(syntax_name)   / sizeof(char)
    , syntax_def    , sizeof(syntax_def)    / sizeof(char)
    , syntax_simple , sizeof(syntax_simple) / sizeof(char)
    , syntax_node   , sizeof(syntax_node)   / sizeof(MIN_REGEX_NODE)
  );

  const int bnf_size = lex_size + parser_size;
  fprintf(stderr, "total bnfsize : %d\n", bnf_size);

  LEX_TOKEN token[1000];
  const char* src_str = "2==(15+20)*203-(42-0)/(0-7*A0b+b)";
  const int token_size = match_lexer(token, sizeof(token)/sizeof(LEX_TOKEN), bnf, lex_size, src_str);
  print_token(stderr, bnf, token, token_size);
}
