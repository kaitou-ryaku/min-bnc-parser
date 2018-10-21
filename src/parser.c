#include "../include/common.h"
#include "../include/lexer.h"
#include "../include/text.h"
#include "../include/bnf.h"
#include "../min-regex/include/min-regex.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// 関数プロトタイプ/*{{{*/
static BNF node_to_bnf(const MIN_REGEX_NODE node, const BNF* bnf);
static void print_parse_tree(FILE *fp, const int pt_size, const PARSE_TREE* pt, const BNF* bnf, const LEX_TOKEN* token);
extern int parse_token_list(
  const   LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
  , const int        pt_max_size
);
static void initialize_parse_tree(
  PARSE_TREE*        pt
  , const int        pt_max_size
);
/*}}}*/

static BNF node_to_bnf(const MIN_REGEX_NODE node, const BNF* bnf) {/*{{{*/
  return bnf[node.symbol + 127];
}/*}}}*/
extern int create_parser(/*{{{*/
  const char*       syntax_str
  , BNF*            bnf
  , char*           work
  , const int       work_max_size
  , char*           name
  , const int       name_max_size
  , char*           def
  , const int       def_max_size
  , char*           simple
  , const int       simple_max_size
  , MIN_REGEX_NODE* node
  , const int       node_max_size
) {

  const int lex_size = bnf[0].lex_size;
  const int syntax_size = read_bnf(syntax_str, &(bnf[lex_size]), bnf[0].total_size, name, name_max_size, def, def_max_size);
  const int bnf_size    = lex_size + syntax_size;

  // 配列サイズを保存
  for (int index=0; index<bnf_size; index++) {
    bnf[index].bnf_size    = bnf_size;
    bnf[index].lex_size    = lex_size;
    bnf[index].syntax_size = syntax_size;
  }

  int simple_seek = 0;
  int node_seek   = 0;

  for (int index=lex_size; index<bnf_size; index++) {
    bnf[index].is_terminal = false;
    bnf[index].node        = &(node[node_seek]);
    bnf[index].simple      = &(simple[simple_seek]);

    int seek      = 0;
    int work_seek = 0;
    const int def_size = strlen(bnf[index].def);
    while (seek < def_size) {
      int begin, end;
      get_next_word_index(bnf[index].def, seek, &begin, &end);
      const char c = (bnf[index].def)[begin];

      if ((c == '(') || (c == '|') || (c == ')') || (c == '*') || (c == '@')) work[work_seek] = c;
      else {
        int cmp_index;
        for (cmp_index=0; cmp_index<bnf_size; cmp_index++) {
          const char* name1 = bnf[index].def;
          const char* name2 = bnf[cmp_index].name;
          if (is_same_word(name1, begin, end, name2, 0, strlen(name2))) {
            work[work_seek] = bnf[cmp_index].alphabet;
            break;
          }
        }

        if (cmp_index >= bnf_size) {
          fprintf(stderr, "ERROR: NO TOKEN OR ATTRIBUTE EXISTS IN LEFT SIDE OF BNF ... ");
          for (int i=begin; i<end; i++) fprintf(stderr, "%c", (bnf[index].def)[i]);
          fprintf(stderr, "\n");
          assert(cmp_index < bnf_size);
        }
      }

      seek = end;
      work_seek++;
    }
    work[work_seek] = '\0';

    simple_seek += simplify_regex_arbitary(
      work
      , 0
      , strlen(work)
      , &(simple[simple_seek])
      , simple_max_size - simple_seek
    );

    node_seek += regex_to_all_node(
      bnf[index].simple
      , bnf[index].node
      , node_max_size - node_seek
    );
  }

  return syntax_size;
}/*}}}*/
extern void syntax_to_dot(/*{{{*/
  FILE*        fp
  , BNF*       bnf
  , const int  lex_size
  , const int  syntax_size
  , const char* fontsize
  , const char* width
  , const char* topic_color
  , const char* boundary_color
  , const char* normal_color
) {

  fprintf( fp, "digraph graphname {\n");
  fprintf( fp, "  graph [rankdir = LR]\n");

  for (int graph_id=lex_size; graph_id < lex_size + syntax_size; graph_id++) {
    const MIN_REGEX_NODE *node = bnf[graph_id].node;

    fprintf( fp, "\n  %05d%05d [ label=\"%s\", fontsize=%s, width=%s, shape=box, fontcolor=\"%s\", color=\"%s\"]\n", graph_id, 99999, bnf[graph_id].name, fontsize, width, boundary_color, boundary_color);
    for (int i=node[0].total-1; i>=0;i--) {
      MIN_REGEX_NODE n = node[i];
      if ((n.in_fst < 0) && (n.in_snd < 0) && (n.out_fst < 0) && (n.out_snd < 0)) continue;

      fprintf( fp, "  ");
      fprintf( fp, "%05d%05d [ "      , graph_id, i);
      if ((n.symbol == '(') || (n.symbol == '|') || (n.symbol == ')') || (n.symbol == '*') || (n.symbol == '@')) {
        fprintf( fp, "label=\"\"");

      } else if ((n.symbol == '^') || (n.symbol == '$')) {
        fprintf( fp, "label=\"%c\", " , n.symbol);
      } else {
        fprintf( fp, "label=\"%s\", " , node_to_bnf(n, bnf).name);
        if (n.symbol + 127 < lex_size) fprintf( fp, "style=filled, fillcolor=\"#C0C0C0\", ");
      }
      fprintf( fp, "fontsize=%s, ", fontsize);
      fprintf( fp, "width=%s, "   , width);

      if (n.is_magick) {
        fprintf( fp, "shape=circle, ");
      } else {
        fprintf( fp, "shape=box, ");
      }

      if (i == 0 || i == 1){
        fprintf( fp, "fontcolor=\"%s\", color=\"%s\"", boundary_color, boundary_color);
      }  else {
        fprintf( fp, "fontcolor=\"%s\", color=\"%s\"", normal_color, normal_color);
      }

      fprintf( fp, "]\n");
    }

    fprintf( fp, "\n");

    fprintf( fp, "  %05d%05d -> %05d%05d [style=invisible]\n", graph_id, 99999, graph_id, 0);
    for (int i=0; i<node[0].total; i++) {
      MIN_REGEX_NODE n = node[i];
      if (n.out_fst >= 0) fprintf( fp, "  %05d%05d -> %05d%05d\n", graph_id, i, graph_id, n.out_fst);
      if (n.out_snd >= 0) fprintf( fp, "  %05d%05d -> %05d%05d\n", graph_id, i, graph_id, n.out_snd);
    }
  }

  fprintf( fp, "}\n");

}/*}}}*/
extern int parse_token_list(/*{{{*/
  const   LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
  , const int        pt_max_size
) {

  initialize_parse_tree(pt, pt_max_size);
  print_parse_tree(stderr, 10, pt, bnf, token);
  return 0;
}/*}}}*/
static void initialize_parse_tree(/*{{{*/
  PARSE_TREE*        pt
  , const int        pt_max_size
) {

  for (int i=0; i<pt_max_size; pt++) {
    pt[i].id                = i;
    pt[i].total_size        = pt_max_size;
    pt[i].used_size         = 0;
    pt[i].bnf_id            = -1;
    pt[i].token_begin_index = -1;
    pt[i].token_end_index   = -1;
    pt[i].up_bnf_node_id    = -1;
    pt[i].up                = -1;
    pt[i].down              = -1;
    pt[i].left              = -1;
    pt[i].right             = -1;
    pt[i].down_total        = -1;
    pt[i].is_terminal       = false;
  }
}/*}}}*/
static void print_parse_tree(FILE *fp, const int pt_size, const PARSE_TREE* pt, const BNF* bnf, const LEX_TOKEN* token) {/*{{{*/
  for (int i=0; i<pt_size;i++) {
    fprintf(stderr, "%02d ", pt[i].id);
    fprintf(stderr, "%s ", bnf[pt[i].bnf_id].name);

    fprintf(stderr, "%02d ", pt[i].up         );
    fprintf(stderr, "%02d ", pt[i].down       );
    fprintf(stderr, "%02d ", pt[i].left       );
    fprintf(stderr, "%02d ", pt[i].right      );
    fprintf(stderr, "%02d ", pt[i].down_total );
    fprintf(stderr, "%02d ", pt[i].is_terminal);

    const int t_begin = token[pt[i].token_begin_index].begin;
    const int t_end   = token[pt[i].token_end_index].end;
    for (int j=t_begin; j<t_end; j++) {
      fprintf(stderr, "%c", (token[0].src)[j]);
    }
    fprintf(stderr, "\n");
  }
}/*}}}*/
