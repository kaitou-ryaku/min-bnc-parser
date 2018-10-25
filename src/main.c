#include "../include/bnf.h"
#include "../include/lexer.h"
#include "../include/syntax.h"
#include "../include/parser.h"
#include <stdio.h>
#include <assert.h>

int main(void) {

  BNF bnf[255]; // char:255文字のアルファベットに対応
  initialize_bnf(bnf, sizeof(bnf)/sizeof(BNF));

  const char* lex_str = ""
    // #include "../bnf/c_lex.bnf"
    #include "../bnf/math_lex.bnf"
  ;

  char           lex_name[2000];
  char           lex_def[2000];
  char           lex_simple[2000];
  MIN_REGEX_NODE lex_node[2000];

  const int lex_size = create_lex(
    lex_str, bnf
    , lex_name   , sizeof(lex_name)   / sizeof(char)
    , lex_def    , sizeof(lex_def)    / sizeof(char)
    , lex_simple , sizeof(lex_simple) / sizeof(char)
    , lex_node   , sizeof(lex_node)   / sizeof(MIN_REGEX_NODE)
  );

  const char* syntax_str = ""
    // #include "../bnf/c_syntax.bnf"
    #include "../bnf/math_syntax.bnf"
  ;

  char           work[2000];
  char           syntax_name[4000];
  char           syntax_def[20000];
  char           syntax_simple[20000];
  MIN_REGEX_NODE syntax_node[2000];

  const int syntax_size = create_syntax(
    syntax_str, bnf
    , work          , sizeof(work)          / sizeof(char)
    , syntax_name   , sizeof(syntax_name)   / sizeof(char)
    , syntax_def    , sizeof(syntax_def)    / sizeof(char)
    , syntax_simple , sizeof(syntax_simple) / sizeof(char)
    , syntax_node   , sizeof(syntax_node)   / sizeof(MIN_REGEX_NODE)
  );

  const int bnf_size = lex_size + syntax_size;
  fprintf(stderr, "total bnfsize : %d\n", bnf_size);

  {
    FILE *fp;
    char *filename = "syntax.dot";
    if ((fp = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "Failed to open %s.\n", filename);
    }
    syntax_to_dot(fp, bnf, "12.0", "0.2", "#FF0000", "#FF0000", "#000000");
    fclose(fp);
  }

  LEX_TOKEN token[1000];
  const char* src_str = ""
    // #include "../bnf/c_source.txt"
    #include "../bnf/math_source.txt"
  ;
  const int token_size = match_lexer(token, sizeof(token)/sizeof(LEX_TOKEN), bnf, src_str);
  print_token(stderr, bnf, token, token_size);

  PARSE_TREE pt[5000];
  static bool memo[255*2000*2000];
  parse_token_list(token, bnf, pt, sizeof(pt)/sizeof(PARSE_TREE), memo, sizeof(memo)/sizeof(bool));

  {
    FILE *fp;
    char *filename = "parse_tree.dot";
    if ((fp = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "Failed to open %s.\n", filename);
    }

    origin_parse_tree_to_dot(fp, 0, pt, bnf, token, "12.0", NULL, "#FF0000", "#000000");

    fclose(fp);
  }

  return 0;
}
