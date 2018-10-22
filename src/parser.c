#include "../include/common.h"
#include "../include/lexer.h"
#include "../include/text.h"
#include "../include/bnf.h"
#include "../min-regex/include/min-regex.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
int TEST=0;

// 関数プロトタイプ/*{{{*/
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
static int parse_match_exact(
  const   int        bnf_index
  , const int        up_bnf_node_index
  , const int        token_begin_index
  , const int        token_end_index
  , const int        pt_parent_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
);
static int parse_match_longest(
  const   int        bnf_index
  , const int        up_bnf_node_index
  , const int        token_begin_index
  , const int        token_end_index
  , const int        pt_parent_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
);
static int parse_syntax_recursive(
  const   int        token_begin_index
  , const int        token_end_index
  , const int        pt_parent_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
);
/*}}}*/
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

  const int syntax_size = read_bnf(syntax_str, bnf, name, name_max_size, def, def_max_size, 3); // 3は(unused 0, meta 1, lex 2, syntax 3)の3
  int simple_seek = 0;
  int node_seek   = 0;

  int index = -1;
  while (1) {
    index = search_bnf_next_syntax(index, bnf);
    if (index < 0) break;

    bnf[index].node   = &(node[node_seek]);
    bnf[index].simple = &(simple[simple_seek]);

    int seek      = 0;
    int work_seek = 0;
    const int def_size = strlen(bnf[index].def);

    while (seek < def_size) {
      int begin, end;
      get_next_word_index(bnf[index].def, seek, &begin, &end);
      const char c = (bnf[index].def)[begin];

      if ((c == '(') || (c == '|') || (c == ')') || (c == '*') || (c == '@')) work[work_seek] = c;
      else {

        int cmp_index = -1;
        while (1) {
          cmp_index = search_bnf_next_lex_or_syntax(cmp_index, bnf);
          if (cmp_index < 0) break;

          const BNF cmp = bnf[cmp_index];
          if (!(is_lex(cmp)) && !(is_syntax(cmp))) continue;

          const char* name1 = bnf[index].name;
          const char* name2 = cmp.name;
          if (is_same_word(name1, 0, strlen(name1), name2, 0, strlen(name2))) {
            work[work_seek] = bnf[cmp_index].alphabet;
            break;
          }
        }

        if (cmp_index < 0) {
          fprintf(stderr, "ERROR: NO TOKEN OR ATTRIBUTE EXISTS IN LEFT SIDE OF BNF ... ");
          for (int i=begin; i<end; i++) fprintf(stderr, "%c", (bnf[index].def)[i]);
          fprintf(stderr, "\n");
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
  FILE*         fp
  , BNF*        bnf
  , const char* fontsize
  , const char* width
  , const char* topic_color
  , const char* boundary_color
  , const char* normal_color
) {

  fprintf( fp, "digraph graphname {\n");
  fprintf( fp, "  graph [rankdir = LR]\n");


  int graph_id = -1;
  while (1) {
    graph_id = search_bnf_next_syntax(graph_id, bnf);
    if (graph_id < 0) break;

    const MIN_REGEX_NODE *node = bnf[graph_id].node;

    fprintf( fp, "\n  %05d%05d [ label=\"%s\", fontsize=%s, width=%s, shape=box, fontcolor=\"%s\", color=\"%s\"]\n"
      , graph_id, 99999, bnf[graph_id].name, fontsize, width, boundary_color, boundary_color);

    for (int i=node[0].total-1; i>=0;i--) {
      MIN_REGEX_NODE n = node[i];
      if ((n.in_fst < 0) && (n.in_snd < 0) && (n.out_fst < 0) && (n.out_snd < 0)) continue;

      fprintf( fp, "  ");
      fprintf( fp, "%05d%05d [ ", graph_id, i);
      if ((n.symbol == '(') || (n.symbol == '|') || (n.symbol == ')') || (n.symbol == '*') || (n.symbol == '@')) {
        fprintf( fp, "label=\"\"");

      } else if ((n.symbol == '^') || (n.symbol == '$')) {
        fprintf( fp, "label=\"%c\", " , n.symbol);
      } else {
        fprintf( fp, "label=\"%s\", " , node_to_bnf(n, bnf).name);
        if (is_lex(node_to_bnf(n, bnf))) fprintf( fp, "style=filled, fillcolor=\"#C0C0C0\", ");
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
static void initialize_parse_tree(/*{{{*/
  PARSE_TREE*        pt
  , const int        pt_max_size
) {
  for (int i=0; i<pt_max_size; i++) {
    pt[i].id                = i;
    pt[i].state             = 0;
    pt[i].total_size        = pt_max_size;
    pt[i].used_size         = 0;
    pt[i].bnf_id            = -1;
    pt[i].up_bnf_node_index = -1;
    pt[i].token_begin_index = -1;
    pt[i].token_end_index   = -1;
    pt[i].up                = -1;
    pt[i].down              = -1;
    pt[i].left              = -1;
    pt[i].right             = -1;
  }
}/*}}}*/
static void print_parse_tree(FILE *fp, const int pt_size, const PARSE_TREE* pt, const BNF* bnf, const LEX_TOKEN* token) {/*{{{*/
  for (int i=0; i<pt_size;i++) {
    fprintf(stderr, "id:%02d ", pt[i].id);
    fprintf(stderr, "name:%s "  , bnf[pt[i].bnf_id].name);
    fprintf(stderr, "state:%02d " , pt[i].state);
    fprintf(stderr, "bnf_id:%02d ", pt[i].bnf_id);
    fprintf(stderr, "up_bnf_node_index:%02d ", pt[i].up_bnf_node_index);
    fprintf(stderr, "up:%02d ", pt[i].up         );
    fprintf(stderr, "down:%02d ", pt[i].down       );
    fprintf(stderr, "left:%02d ", pt[i].left       );
    fprintf(stderr, "right:%02d ", pt[i].right      );

    const int t_begin = token[pt[i].token_begin_index].begin;
    const int t_end   = token[pt[i].token_end_index-1].end;
    for (int j=t_begin; j<t_end; j++) {
      fprintf(stderr, "%c", (token[0].src)[j]);
    }
    fprintf(stderr, "\n");
  }
}/*}}}*/
extern int parse_token_list(/*{{{*/
  const   LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
  , const int        pt_max_size
) {

  initialize_parse_tree(pt, pt_max_size);

  const int begin_bnf_index = search_bnf_next_syntax(-1, bnf);
  pt[0].bnf_id            = begin_bnf_index;
  pt[0].up_bnf_node_index = -1;
  pt[0].state             = 3;
  pt[0].token_begin_index = 0;
  pt[0].token_end_index   = token[0].used_size;
  pt[0].up                = -1;
  pt[0].right             = -1;
  pt[0].left              = -1;

  const int step = parse_syntax_recursive(pt[0].token_begin_index, pt[0].token_end_index, 0, 1, token, bnf, pt);
  fprintf(stderr, "TOTAL PARSE TREE STEP:%d\n", step);
  return 0;
}/*}}}*/
static int parse_match_exact(/*{{{*/
  const   int        bnf_index
  , const int        up_bnf_node_index
  , const int        token_begin_index
  , const int        token_end_index
  , const int        pt_parent_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
) {

  int testtmp = TEST;
  TEST++;
  fprintf(stderr, "\n%d-----------------------------------------------------------------\n", testtmp);
  fprintf(stderr, "LINE:%03d FUNC:%20s bnf_index:%02d up_bnf_node_index:%02d token_begin_index:%02d token_end_index:%02d pt_parent_index:%02d pt_empty_index:%02d", __LINE__, __func__, bnf_index, up_bnf_node_index, token_begin_index, token_end_index, pt_parent_index, pt_empty_index);
  print_parse_tree(stderr, pt_empty_index , pt, bnf, token);

  int step = pt_empty_index;

  int left = pt[pt_parent_index].down;
  if (left >= 0) {
    while (pt[left].right >= 0) {
      left = pt[left].right;
    }
  }

  // 受け取ったBNFがSYNTAXの場合/*{{{*/
  if (is_syntax(bnf[bnf_index])) {
    pt[step].bnf_id            = bnf_index;
    pt[step].up_bnf_node_index = up_bnf_node_index;
    pt[step].state             = 3;
    pt[step].token_begin_index = token_begin_index;
    pt[step].token_end_index   = token_end_index;
    pt[step].up                = pt_parent_index;
    pt[step].left              = left;

    if (left >= 0) pt[left].right = step;
    step++;

    const int new_step = parse_syntax_recursive(token_begin_index, token_end_index, step-1, step, token, bnf, pt);

    if (step == new_step) {
      pt[step].state = 0;
      pt[left].right = -1;
      step--;
    }
  }/*}}}*/

  // 受け取ったBNFがLEXの場合/*{{{*/
  if (is_lex(bnf[bnf_index])) {
    if ( (token_begin_index + 1 == token_end_index)
      && (bnf[bnf_index].kind == token[token_begin_index].kind)
    ) {

      pt[step].bnf_id            = bnf_index;
      pt[step].up_bnf_node_index = up_bnf_node_index;
      pt[step].state             = 2;
      pt[step].token_begin_index = token_begin_index;
      pt[step].token_end_index   = token_end_index;
      pt[step].up                = pt_parent_index;
      pt[step].left              = left;

      if (left >= 0) pt[left].right = step;
      step++;
    }
  }/*}}}*/

  // 受け取ったBNFがMETAの場合/*{{{*/
  if (is_meta(bnf[bnf_index])) {
    if (token_begin_index == token_end_index) {

      pt[step].bnf_id            = bnf_index;
      pt[step].up_bnf_node_index = up_bnf_node_index;
      pt[step].state             = 1;
      pt[step].token_begin_index = token_begin_index;
      pt[step].token_end_index   = token_end_index;
      pt[step].up                = pt_parent_index;
      pt[step].left              = left;

      if (left >= 0) pt[left].right = step;
      step++;
    }
  }/*}}}*/

  fprintf(stderr, "\nLINE:%03d FUNC:%20s end:\n",__LINE__,  __func__);
  print_parse_tree(stderr, step, pt, bnf, token);
  fprintf(stderr, "%d-----------------------------------------------------------------\n", testtmp);

  return step;
}/*}}}*/
static int parse_match_longest(/*{{{*/
  const   int        bnf_index
  , const int        up_bnf_node_index
  , const int        token_begin_index
  , const int        token_end_index
  , const int        pt_parent_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
) {

  int testtmp = TEST;
  TEST++;
  fprintf(stderr, "\n%d-----------------------------------------------------------------\n", testtmp);
  fprintf(stderr, "LINE:%03d FUNC:%20s bnf_index:%02d up_bnf_node_index:%02d token_begin_index:%02d token_end_index:%02d pt_parent_index:%02d pt_empty_index:%02d\n", __LINE__, __func__, bnf_index, up_bnf_node_index, token_begin_index, token_end_index, pt_parent_index, pt_empty_index);
  print_parse_tree(stderr, pt_empty_index , pt, bnf, token);

  int step = pt_empty_index;
  for (int tmp_end=token_end_index; tmp_end <= token_begin_index; tmp_end++) {
    const int new_step = parse_match_exact(bnf_index, up_bnf_node_index, token_begin_index, tmp_end, pt_parent_index, step, token, bnf, pt);
    if (step < new_step) {
      step = new_step;
      break;
    }
  }

  fprintf(stderr, "\nLINE:%03d FUNC:%20s end:\n",__LINE__,  __func__);
  print_parse_tree(stderr, step, pt, bnf, token);
  fprintf(stderr, "%d-----------------------------------------------------------------\n", testtmp);

  return step;
}/*}}}*/
static int parse_syntax_recursive(/*{{{*/
  const   int        token_begin_index
  , const int        token_end_index
  , const int        pt_parent_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
) {

  int testtmp = TEST;
  TEST++;
  fprintf(stderr, "\n%d-----------------------------------------------------------------\n", testtmp);
  fprintf(stderr, "LINE:%03d FUNC:%s token_begin_index:%02d token_end_index:%02d pt_parent_index:%02d pt_empty_index:%02d\n", __LINE__, __func__, token_begin_index, token_end_index, pt_parent_index, pt_empty_index);
  print_parse_tree(stderr, pt_empty_index , pt, bnf, token);

  int step = pt_empty_index;
  const int up = pt_parent_index;
  assert(is_syntax(bnf[pt[up].bnf_id]));

  const MIN_REGEX_NODE* node = bnf[pt[up].bnf_id].node;

  int begin   = token_begin_index;
  int end     = token_end_index;
  int current = 0;

  while (1) {

    // マッチング
    const int bnf_index = node_to_bnf_id(node[current], bnf);
    const int new_step  = parse_match_longest(bnf_index, current, begin, end, up, step, token, bnf, pt);

    // マッチ成功 -> out_fstノードへ移動
    if (step < new_step) {
      // 終点ノードでマッチに成功していれば、ループを抜ける
      if (node[current].is_magick && node[current].symbol == '$') break;

      begin   = pt[step].token_end_index;
      end     = token_end_index;
      step    = new_step;
      current = node[current].out_fst;

    // マッチ失敗 -> バックトラック
    } else {
      // 始点ノードでマッチに失敗していれば、ループを抜ける
      if (node[current].is_magick && node[current].symbol == '^') break;

      int left = pt[pt_parent_index].down;
      if (left >= 0) {
        while (pt[left].right >= 0) {
          left = pt[left].right;
        }
      }

      MIN_REGEX_NODE left_node = node[pt[left].up_bnf_node_index];

      // バックトラックで二股の下のノードが未探索の場合、そこに移動
      if ((left_node.out_fst == current) && (left_node.out_snd >= 0)) {
        begin   = pt[left].token_end_index;
        end     = token_end_index;
        current = left_node.out_snd;

      // バックトラックで二股の下のノードがないとき、左のノードに移動
      } else {
        begin   = pt[left].token_begin_index;
        end     = pt[left].token_end_index-1;
        step    = new_step;
        current = pt[left].up_bnf_node_index;
      }
    }
  }

  fprintf(stderr, "\nLINE:%03d FUNC:%20s end:\n",__LINE__,  __func__);
  print_parse_tree(stderr, step, pt, bnf, token);
  fprintf(stderr, "%d-----------------------------------------------------------------\n", testtmp);

  return step;
}/*}}}*/
