#include "../include/lexer.h"
#include "../min-regex/include/min-regex.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int count_lex_line_total(const char *lex_bnf);
static void get_next_lex_left_index(const char* lex_bnf, const int next_line, int* begin, int* end);
static void get_next_lex_right_index(const char* lex_bnf, const int next_line, int* begin, int* end);

static int count_lex_line_total(const char *lex_bnf) {/*{{{*/
  int line = 0;
  int seek = 0;
  while (lex_bnf[seek] != '\0') {
    if (lex_bnf[seek] == '\n') line++;
    seek++;
  }
  return line;
}/*}}}*/
static void get_next_lex_left_index(const char* lex_bnf, const int next_line, int* begin, int* end) {/*{{{*/
  int seek = 0;
  int line = 0;
  while (line < next_line) {
    if (lex_bnf[seek] == '\n') line++;
    seek++;
  }

  while (lex_bnf[seek] == ' ') seek++;
  (*begin) = seek;

  while (lex_bnf[seek] != ' ') seek++;
  (*end) = seek;
}/*}}}*/
static void get_next_lex_right_index(const char* lex_bnf, const int next_line, int* begin, int* end) {/*{{{*/
  int seek = 0;
  int line = 0;
  while (line < next_line) {
    if (lex_bnf[seek] == '\n') line++;
    seek++;
  }

  while (lex_bnf[seek] != ':') seek++;
  seek++;

  while (lex_bnf[seek] == ' ') seek++;
  (*begin) = seek;

  while (lex_bnf[seek] != '\n') seek++;
  (*end) = seek;
}/*}}}*/

extern void lexer(const char* lex_bnf, const char *src_str) {
  fprintf(stderr, "LEXICAL ANALYSIS OF\n%s\n", src_str);
  fprintf(stderr, "%s\n", lex_bnf);

  LEX_BNF lex[100];
  MIN_REGEX_NODE node[30000];

  const int line_total = count_lex_line_total(lex_bnf);
  for (int line=0; line<line_total; line++) {
    lex[line].kind = line;
    lex[line].str  = lex_bnf;
    lex[line].node = &(node[line*300]);

    int begin, end;

    get_next_lex_left_index(lex_bnf, line, &begin, &end);
    lex[line].name_begin = begin;
    lex[line].name_end   = end;

    get_next_lex_right_index(lex_bnf, line, &begin, &end);
    lex[line].bnf_begin = begin;
    lex[line].bnf_end   = end;

    regex_to_all_node_arbitary(lex[line].str, lex[line].bnf_begin, lex[line].bnf_end, lex[line].node, 300);
  }

  LEX_TOKEN token[100];
  int token_id = 0;
  int index = 0;
  while (index < strlen(src_str)) {
    const char *rest = &(src_str[index]);

    for (int line=0; line<line_total; line++) {
      LEX_BNF l = lex[line];
      MIN_REGEX_MATCH match[200];
      int delta = forward_longest_match( rest, l.node, match, 200);

      if (delta > 0) {
        token[token_id].id     = token_id;
        token[token_id].kind   = line;
        token[token_id].src    = src_str;
        token[token_id].begin  = index;
        token[token_id].end    = index+delta;

        for (int j=0; j<index; j++) fprintf(stderr, " ");
        for (int j=0; j<delta; j++) fprintf(stderr, "%c", token[token_id].src[index+j]);
        for (int j=0; j<strlen(src_str)-index-delta; j++) fprintf(stderr, " ");

        fprintf(stderr, " | ");
        for (int j=l.name_begin; j<l.name_end; j++) fprintf(stderr, "%c", (l.str)[j]);
        fprintf(stderr, "\n");

        index = index + delta;
        token_id++;
        break;
      }
    }
  }
}
