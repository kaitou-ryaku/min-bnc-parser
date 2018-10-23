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
static void initialize_parse_tree_unit(
  PARSE_TREE*        pt
  , const int        index
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
extern int create_syntax(/*{{{*/
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

          const char* name2 = bnf[cmp_index].name;
          if (is_same_word(bnf[index].def, begin, end, name2, 0, strlen(name2))) {
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
    pt[i].total_size = pt_max_size;
    initialize_parse_tree_unit(pt, i);
  }
}/*}}}*/
static void initialize_parse_tree_unit(/*{{{*/
  PARSE_TREE*        pt
  , const int        index
) {
  pt[index].id                = index;
  pt[index].state             = 0;
  pt[index].used_size         = 0;
  pt[index].bnf_id            = -1;
  pt[index].up_bnf_node_index = -1;
  pt[index].token_begin_index = -1;
  pt[index].token_end_index   = -1;
  pt[index].up                = -1;
  pt[index].down              = -1;
  pt[index].left              = -1;
  pt[index].right             = -1;
}/*}}}*/
static void print_parse_tree(FILE *fp, const int pt_size, const PARSE_TREE* pt, const BNF* bnf, const LEX_TOKEN* token) {/*{{{*/
  for (int i=0; i<pt_size;i++) {
    fprintf(fp, "id %02d ", pt[i].id);
    fprintf(fp, "name %20s "  , bnf[pt[i].bnf_id].name);
    fprintf(fp, "state %d " , pt[i].state);
    fprintf(fp, "bnf_id %03d ", pt[i].bnf_id);
    fprintf(fp, "up_bnf_node_index %02d ", pt[i].up_bnf_node_index);
    fprintf(fp, "up %02d ", pt[i].up         );
    fprintf(fp, "down %02d ", pt[i].down       );
    fprintf(fp, "left %02d ", pt[i].left       );
    fprintf(fp, "right %02d ", pt[i].right      );
    fprintf(fp, "token [%02d-%02d] ", pt[i].token_begin_index, pt[i].token_end_index);

    const int t_begin = token[pt[i].token_begin_index].begin;
    const int t_end   = token[pt[i].token_end_index-1].end;
    for (int j=t_begin; j<t_end; j++) {
      fprintf(fp, "%c", (token[0].src)[j]);
    }
    fprintf(fp, "\n");
  }
}/*}}}*/
extern void parse_tree_to_dot(FILE *fp, const int pt_size, const PARSE_TREE* pt, const BNF* bnf, const LEX_TOKEN* token, const char* fontsize) {/*{{{*/

  fprintf( fp, "digraph graphname {\n");
  fprintf( fp, "  graph [rankdir = LR]\n");
  fprintf(fp, "\n");

  for (int i=0; i<pt_size;i++) {
    fprintf( fp, "  %05d [ fontsize=%s, shape=box, ", pt[i].id, fontsize);
    fprintf( fp, "label=\"%s\\n", bnf[pt[i].bnf_id].name);
    const int t_begin = token[pt[i].token_begin_index].begin;
    const int t_end   = token[pt[i].token_end_index-1].end;
    for (int j=t_begin; j<t_end; j++) {
      fprintf(fp, "%c", (token[0].src)[j]);
    }
    fprintf(fp, "\"]\n");
  }

  fprintf(fp, "\n");

  for (int i=0; i<pt_size;i++) {
    const int up    = pt[i].up;
    const int down  = pt[i].down;
    const int left  = pt[i].left;
    const int right = pt[i].right;

    if (up    >= 0) fprintf( fp, "  %05d -> %05d\n", pt[i].id, up   );
    if (down  >= 0) fprintf( fp, "  %05d -> %05d\n", pt[i].id, down );
    if (left  >= 0) fprintf( fp, "  %05d -> %05d\n", pt[i].id, left );
    if (right >= 0) fprintf( fp, "  %05d -> %05d\n", pt[i].id, right);
  }

  fprintf(fp, "}\n");
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
  print_parse_tree(stderr, step, pt, bnf, token);
  return step;
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

  if (token_begin_index > token_end_index) return pt_empty_index;

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
    pt[step].down              = -1;
    pt[step].left              = left;
    pt[step].right             = -1;

    if (left < 0) pt[pt_parent_index].down = step;
    else          pt[left].right = step;
    step++;

    const int new_step = parse_syntax_recursive(token_begin_index, token_end_index, step-1, step, token, bnf, pt);

    if (step == new_step) {
      initialize_parse_tree_unit(pt, step);
      pt[left].right = -1;
      step--;
    } else {
      step = new_step;
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
      pt[step].down              = -1;
      pt[step].left              = left;
      pt[step].right             = -1;

      if (left < 0) pt[pt_parent_index].down = step;
      else          pt[left].right = step;
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
      pt[step].down              = -1;
      pt[step].left              = left;
      pt[step].right             = -1;

      if (left < 0) pt[pt_parent_index].down = step;
      else          pt[left].right = step;
      step++;
    }
  }/*}}}*/

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

  int step = pt_empty_index;
  for (int tmp_end=token_end_index; token_begin_index <= tmp_end; tmp_end--) {
    const int new_step = parse_match_exact(bnf_index, up_bnf_node_index, token_begin_index, tmp_end, pt_parent_index, step, token, bnf, pt);
    if (step < new_step) {
      step = new_step;
      break;
    }
  }

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

  int step = pt_empty_index;
  const int up = pt_parent_index;
  assert(is_syntax(bnf[pt[up].bnf_id]));

  const MIN_REGEX_NODE* node = bnf[pt[up].bnf_id].node;

  int begin   = token_begin_index;
  int end     = token_end_index;
  int current = 0;
  int max_used_index = pt_empty_index;

  while (1) {
    if (max_used_index < step) max_used_index = step;

    // マッチング
    const int bnf_index = node_to_bnf_id(node[current], bnf);
    const int new_step  = parse_match_longest(bnf_index, current, begin, end, up, step, token, bnf, pt);

    // フラグ定義/*{{{*/
    const MIN_REGEX_NODE current_node = node[current];
    const PARSE_TREE     current_pt   = pt[step];

    const bool is_match = (step < new_step);
    const bool is_end   = (current_node.is_magick) && (current_node.symbol == '$');
    const bool is_all   = (current_pt.token_begin_index == token_end_index);

    bool is_again = false;
    int check_index = pt[pt_parent_index].down;
    if (check_index >= 0) {
      while (pt[check_index].right >= 0) {
        if ( (current_pt.token_begin_index == pt[check_index].token_begin_index)
          && (current_pt.token_end_index   == pt[check_index].token_end_index) // これは不必要
          && (current_pt.up_bnf_node_index == pt[check_index].up_bnf_node_index)
        ) {
          is_again = true;
          break;
        }
        check_index = pt[check_index].right;
      }
    }/*}}}*/

    // マッチ成功 ($ノードの場合は、トークンを残してない条件も必要) -> out_fstノードへ移動/*{{{*/
    if (is_match && (!is_again) && ((!is_end) || (is_end && is_all))) {

      // $ノードでトークンを残さずマッチに成功した場合は、ループを抜ける
      if (is_end && is_all) {
        step = new_step;
        break;
      }

      // $ノードでない場合は、トークンを更新して次のノードに移動
      begin   = pt[step].token_end_index;
      end     = token_end_index;
      step    = new_step;
      current = node[current].out_fst;
    }/*}}}*/

    // マッチ失敗 ($ノードで、トークンを残したままマッチした場合を含む) -> バックトラック/*{{{*/
    else {
      int right = pt[pt_parent_index].down;
      if (right >= 0) {
        while (pt[right].right >= 0) {
          right = pt[right].right;
        }
      }

      // $ノードでトークンを残したままマッチした場合 -> (最も右に生成済みの)$ノードの、一つ左のノードでバックトラック候補にする
      if (is_end) {
        int tmp = pt[right].left;
        initialize_parse_tree_unit(pt, right);
        right = tmp;
        pt[right].right = -1; // 次にptの最右を走査するとき、$に繋がらないようにする
      }

      MIN_REGEX_NODE right_node = node[pt[right].up_bnf_node_index];

      // 最右のノードが分岐ノードだった -> 分岐の下側のノードが未探索の場合、そこをバックトラック
      if ((right_node.out_fst == current) && (right_node.out_snd >= 0)) {
        begin   = pt[right].token_end_index;
        end     = token_end_index;
        current = right_node.out_snd;

      // 最右のノードが分岐ノードでも$ノードでもない -> 最右のノードをバックトラック
      } else {
        begin   = pt[right].token_begin_index;
        end     = pt[right].token_end_index-1;
        step    = right;
        current = pt[right].up_bnf_node_index;

        // バックトラックするノードは、マッチするかどうか不明なので、左のノードからは繋がらないようにしておく
        if (pt[right].left >= 0) {
          pt[pt[right].left].right = -1;
        }

        // バックトラック先が^ノードになると、失敗とみなしてループを抜ける
        if (node[current].is_magick && node[current].symbol == '^') {
          pt[pt_parent_index].down = -1;

          for (int i=pt_empty_index; i<=max_used_index; i++) initialize_parse_tree_unit(pt, i);
          break;
        }
      }
    }/*}}}*/
  }

  return step;
}/*}}}*/
