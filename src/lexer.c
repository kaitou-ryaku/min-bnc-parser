#include "../include/common.h"
#include "../include/lexer.h"
#include "../include/text.h"
#include "../include/bnf.h"
#include "../min-regex/include/min-regex.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern int create_lexer(/*{{{*/
  const char*       lex_str
  , BNF*            bnf
  , const int       bnf_max_size
  , char*           name
  , const int       name_max_size
  , char*           def
  , const int       def_max_size
  , char*           simple
  , const int       simple_max_size
  , MIN_REGEX_NODE* node
  , const int       node_max_size
) {

  const int lex_size = read_bnf(lex_str, bnf, bnf_max_size, name, name_max_size, def, def_max_size);
  int simple_seek = 0;
  int node_seek   = 0;

  for (int index=0; index<lex_size; index++) {
    bnf[index].node    = &(node[node_seek]);
    bnf[index].simple  = &(simple[simple_seek]);

    simple_seek += simplify_regex_arbitary(
      bnf[index].bnf_str
      , bnf[index].def_begin
      , bnf[index].def_end
      , &(simple[simple_seek])
      , simple_max_size - simple_seek
    );

    node_seek += regex_to_all_node(
      bnf[index].simple
      , bnf[index].node
      , node_max_size - node_seek
    );
  }

  return lex_size;
}/*}}}*/
extern int match_lexer(/*{{{*/
  LEX_TOKEN*    token
  , const int   token_max_size
  , const BNF*  bnf
  , const int   bnf_size
  , const char* src_str
) {
  fprintf(stderr, "LEXICAL ANALYSIS OF\n%s\n", src_str);

  int token_id = 0;
  int index = 0;
  while (index < strlen(src_str)) {
    const char *rest = &(src_str[index]);

    for (int line=0; line<bnf_size; line++) {
      BNF     l = bnf[line];
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
  , BNF*       bnf
  , LEX_TOKEN* token
  , const int  token_size
) {

  for (int token_id=0; token_id<token_size; token_id++) {
    LEX_TOKEN t = token[token_id];
    BNF       l = bnf[t.kind];

    for (int j=0; j<t.begin; j++) fprintf(fp, " ");
    for (int j=0; j<t.end-t.begin; j++) fprintf(fp, "%c", t.src[t.begin+j]);
    for (int j=0; j<strlen(t.src)-t.end; j++) fprintf(fp, " ");

    fprintf(fp, " | %20s", l.name);
    fprintf(fp, " | %20s", l.def);
    fprintf(fp, " | %20s", l.simple);
    fprintf(fp, "\n");
  }
}/*}}}*/
