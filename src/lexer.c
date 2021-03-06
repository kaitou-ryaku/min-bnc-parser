#include "../include/common.h"
#include "../include/lexer.h"
#include "../include/text.h"
#include "../include/bnf.h"
#include "../min-regex/include/min-regex.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern int create_lex(/*{{{*/
  const char*       lex_str
  , BNF*            bnf
  , char*           name
  , const int       name_max_size
  , char*           def
  , const int       def_max_size
  , char*           simple
  , const int       simple_max_size
  , MIN_REGEX_NODE* node
  , const int       node_max_size
) {

  const int lex_size = read_bnf(lex_str, bnf, name, name_max_size, def, def_max_size, PT_STATE_LEX); // 2は(unused 0, meta 1, lex 2, syntax 3)の2
  int simple_seek = 0;
  int node_seek   = 0;

  int index = -1;
  while (1) {
    index = search_bnf_next_lex(index, bnf);
    if (index < 0) break;

    bnf[index].node   = &(node[node_seek]);
    bnf[index].simple = &(simple[simple_seek]);

    const BNF b = bnf[index];
    simple_seek += simplify_regex_arbitary( b.bnf_str, b.def_begin, b.def_end, &(simple[simple_seek]), simple_max_size - simple_seek);

    const int node_size = regex_to_all_node( b.simple, b.node, node_max_size - node_seek);
    bnf[index].node_size = node_size;
    node_seek += node_size;
  }

  return lex_size;
}/*}}}*/
extern int match_lexer(/*{{{*/
  LEX_TOKEN*    token
  , const int   token_max_size
  , char*       name_array
  , const int   name_array_max_size
  , const BNF*  bnf
  , const char* src_str
) {

  fprintf(stderr, "LEXICAL ANALYSIS OF\n%s\n", src_str);

  int token_id = 0;
  int seek     = 0;
  int name_used= 0;
  while (seek  < strlen(src_str)) {
    const char *rest = &(src_str[seek ]);

    // inta を int aと誤判定しないようにidentifierかどうか常に判定する
    int index_identifier = 0;
    while (index_identifier >= 0) {
      if (strcmp(bnf[index_identifier].name, "identifier") == 0) break;
      index_identifier = search_bnf_next_lex(index_identifier, bnf);
    }

    int delta_identifier;
    MIN_REGEX_MATCH match_identifier[200];
    if (index_identifier >= 0) {
      delta_identifier = forward_longest_match( rest, bnf[index_identifier].node, match_identifier, 200);
    } else {
      delta_identifier = -1;
    }

    int index = 0;
    while (index >= 0) {
      BNF l = bnf[index];
      MIN_REGEX_MATCH match[200];
      int delta = forward_longest_match( rest, l.node, match, 200);

      if (delta > 0) {
        // skipノードの場合は登録しない
        if ((strcmp(l.name, "skip") == 0) || (strcmp(l.name, "SKIP") == 0)) {
          seek = seek + delta;
          break;
        }

        // 記号類にマッチ -> delta_identifier == -1, delta > 0 --> 登録
        // int にマッチ   -> delta_identifier == 3, delta == 3 --> 登録
        // intaにマッチ   -> delta_identifier == 4, delta == 3 --> 登録しない
        if (delta_identifier == -1 || delta == delta_identifier) {
          token[token_id].id    = token_id;
          token[token_id].kind  = index;
          token[token_id].src   = src_str;
          token[token_id].begin = seek ;
          token[token_id].end   = seek +delta;
          token[token_id].name  = &(name_array[name_used]);

          assert(name_used+delta+1 < name_array_max_size);
          for (int name_seek=0; name_seek<delta; name_seek++) {
            name_array[name_used+name_seek] = src_str[seek+name_seek];
          }
          name_array[name_used+delta] = '\0';
          name_used += delta+1;

          seek = seek + delta;
          token_id++;
          assert(token_id < token_max_size);
          break;
        }
      }
      index = search_bnf_next_lex(index, bnf);
    }
  }

  for (int i=0; i<token_max_size; i++) {
    token[i].total_size = token_max_size;
    token[i].used_size  = token_id;
  }

  return token_id;
}/*}}}*/
extern void print_token_name (/*{{{*/
  FILE*       fp
  , LEX_TOKEN t
) {
  for (int j=0; j<t.end-t.begin; j++) fprintf(fp, "%c", t.src[t.begin+j]);
}/*}}}*/
extern void print_token_long_line(/*{{{*/
  FILE*        fp
  , BNF*       bnf
  , LEX_TOKEN* token
  , const int  token_size
) {

  for (int token_id=0; token_id<token_size; token_id++) {
    LEX_TOKEN t = token[token_id];
    BNF       l = bnf[t.kind];

    for (int j=0; j<t.begin; j++) fprintf(fp, " ");
    print_token_name(fp, t);
    for (int j=0; j<strlen(t.src)-t.end; j++) fprintf(fp, " ");

    fprintf(fp, " | %s", l.name);
    // fprintf(fp, " | %20s", l.def);
    // fprintf(fp, " | %20s", l.simple);
    fprintf(fp, "\n");
  }
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
    fprintf(fp, "%-20s | ", l.name);
    print_token_name(fp, t);
    fprintf(fp, "\n");
  }
}/*}}}*/
extern bool is_token_kind(/*{{{*/
  const char* name
  , const LEX_TOKEN token
  , const BNF* bnf
) {
  return (0 == strcmp(bnf[token.kind].name, name));
}/*}}}*/
