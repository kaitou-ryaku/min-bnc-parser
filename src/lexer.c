#include "../include/lexer.h"
#include "../include/text.h"
#include "../min-regex/include/min-regex.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern int create_lexer(/*{{{*/
  const char*       bnf_str
  , LEX_BNF*        lex
  , const int       lex_max_size
  , char*           name
  , const int       name_max_size
  , char*           bnf
  , const int       bnf_max_size
  , char*           simple
  , const int       simple_max_size
  , MIN_REGEX_NODE* node
  , const int       node_max_size
) {

  int name_seek   = 0;
  int bnf_seek    = 0;
  int simple_seek = 0;
  int node_seek   = 0;

  const int line_total = count_line_total(bnf_str);
  assert(line_total < lex_max_size);
  for (int line=0; line<line_total; line++) {
    lex[line].kind   = line;
    lex[line].str    = bnf_str;
    lex[line].node   = &(node[node_seek]);
    lex[line].name   = &(name[name_seek]);
    lex[line].bnf    = &(bnf[bnf_seek]);
    lex[line].simple = &(simple[simple_seek]);

    int begin, end;
    get_next_left_index(bnf_str, line, &begin, &end);
    lex[line].name_begin = begin;
    lex[line].name_end   = end;
    assert(name_seek + end - begin + 1 < name_max_size);

    for (int seek=0; seek<end-begin; seek++) {
      name[name_seek + seek] = bnf_str[begin + seek];
    }
    name[name_seek + end - begin] = '\0';
    name_seek += end - begin + 1;

    get_next_right_index(bnf_str, line, &begin, &end);
    lex[line].bnf_begin = begin;
    lex[line].bnf_end   = end;
    assert(bnf_seek + end - begin + 1 < bnf_max_size);

    for (int seek=0; seek<end-begin; seek++) {
      bnf[bnf_seek + seek] = bnf_str[begin + seek];
    }
    bnf[bnf_seek + end - begin] = '\0';
    bnf_seek += end - begin + 1;

    simple_seek += simplify_regex_arbitary(lex[line].str, lex[line].bnf_begin, lex[line].bnf_end, &(simple[simple_seek]), simple_max_size - simple_seek);

    node_seek += regex_to_all_node(lex[line].simple, lex[line].node, node_max_size - node_seek);
  }

  return line_total;
}/*}}}*/
extern int match_lexer(/*{{{*/
  LEX_TOKEN*       token
  , const int      token_max_size
  , const LEX_BNF* lex
  , const int      lex_size
  , const char*    src_str
) {
  fprintf(stderr, "LEXICAL ANALYSIS OF\n%s\n", src_str);

  int token_id = 0;
  int index = 0;
  while (index < strlen(src_str)) {
    const char *rest = &(src_str[index]);

    for (int line=0; line<lex_size; line++) {
      LEX_BNF l = lex[line];
      MIN_REGEX_MATCH match[200];
      int delta = forward_longest_match( rest, l.node, match, 200);

      if (delta > 0) {
        token[token_id].id     = token_id;
        token[token_id].kind   = line;
        token[token_id].src    = src_str;
        token[token_id].begin  = index;
        token[token_id].end    = index+delta;

        index = index + delta;
        token_id++;
        assert(token_id < token_max_size);
        break;
      }
    }
  }

  return token_id;
}/*}}}*/
extern void print_token(/*{{{*/
  FILE*        fp
  , LEX_BNF*   lex
  , const int  lex_size
  , LEX_TOKEN* token
  , const int  token_size
) {

  for (int token_id=0; token_id<token_size; token_id++) {
    LEX_TOKEN t = token[token_id];
    LEX_BNF   l = lex[t.kind];

    for (int j=0; j<t.begin; j++) fprintf(fp, " ");
    for (int j=0; j<t.end-t.begin; j++) fprintf(fp, "%c", t.src[t.begin+j]);
    for (int j=0; j<strlen(t.src)-t.end; j++) fprintf(fp, " ");

    fprintf(fp, " | %20s", l.name);
    fprintf(fp, " | %20s", l.bnf);
    fprintf(fp, " | %20s", l.simple);
    fprintf(fp, "\n");
  }
}/*}}}*/
