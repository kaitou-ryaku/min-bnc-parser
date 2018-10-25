#include "../include/common.h"
#include "../include/bnf.h"
#include "../include/syntax.h"
#include "../min-regex/include/min-regex.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int hoge=0;
// 関数プロトタイプ/*{{{*/
static void print_from_token_to_token(FILE *fp, const int token_begin_index, const int token_end_index, const LEX_TOKEN* token);
static void print_parse_tree(FILE *fp, const int pt_size, const PARSE_TREE* pt, const BNF* bnf, const LEX_TOKEN* token);
static void initialize_parse_tree(
  PARSE_TREE*        pt
  , const int        pt_max_size
);
static void initialize_parse_tree_unit(
  PARSE_TREE*        pt
  , const int        index
);
static bool is_same_pt_exist(const int target_pt_id, const int origin_pt_id, const PARSE_TREE* pt);
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
  , bool*            memo
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
  , bool*            memo
);
static int parse_syntax_recursive(
  const   int        token_begin_index
  , const int        token_end_index
  , const int        pt_parent_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
  , bool*            memo
);
static int delete_parse_tree_meta_all(const int pt_size, PARSE_TREE* pt, const BNF* bnf);
static void delete_parse_tree_single(const int id, PARSE_TREE* pt);
static void origin_parse_tree_to_dot_recursive(FILE *fp, const int origin, const PARSE_TREE* pt, const BNF* bnf, const LEX_TOKEN* token, const char* fontsize, const char* meta_color, const char* lex_color, const char* syntax_color);
static int bnf_token_to_memo_index(const int bnf_id, const int token_begin_index, const int token_end_index, const LEX_TOKEN* token);
/*}}}*/
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
static void print_from_token_to_token(FILE *fp, const int token_begin_index, const int token_end_index, const LEX_TOKEN* token) {/*{{{*/
  if (token == NULL) return;

  int begin;
  if      (token_begin_index < 0)                   begin = 0;
  else if (token_begin_index >= token[0].used_size) begin = -1;
  else                                              begin = token[token_begin_index].begin;

  int end;
  if      (token_end_index <= 0)                  end = -1;
  else if (token_end_index >  token[0].used_size) end = token[token[0].used_size-1].end;
  else                                            end = token[token_end_index-1].end;

  if ((begin >= 0) && (end >= 0)) {
    for (int j=begin; j<end; j++) {
      fprintf(fp, "%c", (token[0].src)[j]);
    }
  }
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
    print_from_token_to_token(fp, pt[i].token_begin_index, pt[i].token_end_index, token);
    fprintf(fp, "\n");
  }
}/*}}}*/
extern void all_parse_tree_to_dot(FILE *fp, const int pt_size, const PARSE_TREE* pt, const BNF* bnf, const LEX_TOKEN* token, const char* fontsize) {/*{{{*/

  fprintf( fp, "digraph graphname {\n");
  fprintf( fp, "  graph\n");
  fprintf(fp, "\n");

  for (int i=0; i<pt_size;i++) {
    fprintf( fp, "  %05d [ fontsize=%s, shape=box, ", pt[i].id, fontsize);
    fprintf( fp, "label=\"%s\\n", bnf[pt[i].bnf_id].name);
    print_from_token_to_token(fp, pt[i].token_begin_index, pt[i].token_end_index, token);
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
extern void origin_parse_tree_to_dot(FILE *fp, const int origin, const PARSE_TREE* pt, const BNF* bnf, const LEX_TOKEN* token, const char* fontsize, const char* meta_color, const char* lex_color, const char* syntax_color) {/*{{{*/
  fprintf( fp, "digraph graphname {\n");
  fprintf(fp, "\n");
  origin_parse_tree_to_dot_recursive(fp, origin, pt, bnf, token, fontsize, meta_color, lex_color, syntax_color);
  fprintf(fp, "}\n");
}/*}}}*/
static void origin_parse_tree_to_dot_recursive(FILE *fp, const int origin, const PARSE_TREE* pt, const BNF* bnf, const LEX_TOKEN* token, const char* fontsize, const char* meta_color, const char* lex_color, const char* syntax_color) {/*{{{*/

  fprintf( fp, "  %05d [ fontsize=%s, shape=box, ", pt[origin].id, fontsize);
  if      (is_meta(bnf[pt[origin].bnf_id]))   fprintf(fp, "color=\"%s\", ", meta_color);
  else if (is_lex(bnf[pt[origin].bnf_id]))    fprintf(fp, "color=\"%s\", ", lex_color);
  else if (is_syntax(bnf[pt[origin].bnf_id])) fprintf(fp, "color=\"%s\", ", syntax_color);
  fprintf( fp, "label=\"%s\\n", bnf[pt[origin].bnf_id].name);

  print_from_token_to_token(fp, pt[origin].token_begin_index, pt[origin].token_end_index, token);

  fprintf(fp, "\"]\n");

  if (pt[origin].up >= 0) fprintf( fp, "  %05d -> %05d\n", pt[origin].up, origin);

  if (pt[origin].right >= 0) {
    origin_parse_tree_to_dot_recursive(fp, pt[origin].right, pt, bnf, token, fontsize, meta_color, lex_color, syntax_color);
  }

  if (pt[origin].down  >= 0) {
    origin_parse_tree_to_dot_recursive(fp, pt[origin].down, pt, bnf, token, fontsize, meta_color, lex_color, syntax_color);
  }
}/*}}}*/
extern int parse_token_list(/*{{{*/
  const   LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
  , const int        pt_max_size
  , bool*            memo
  , const int        memo_max_size
) {

  const int memo_size = 255*token[0].used_size*token[0].used_size;
  assert(memo_size < memo_max_size);
  for (int i=0; i<memo_max_size; i++) memo[i] = true;

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

  const int step = parse_syntax_recursive(pt[0].token_begin_index, pt[0].token_end_index, 0, 1, token, bnf, pt, memo);
  fprintf(stderr, "TOTAL PARSE TREE STEP:%d\n", step);
  print_parse_tree(stderr, step, pt, bnf, token);
  delete_parse_tree_meta_all(step, pt, bnf);

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
  , bool*            memo
) {

  if (token_begin_index > token_end_index) return pt_empty_index;

  const int memo_index = bnf_token_to_memo_index(bnf_index, token_begin_index, token_end_index, token);
  if (!(memo[memo_index])) return pt_empty_index;

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

    const int new_step = parse_syntax_recursive(token_begin_index, token_end_index, step-1, step, token, bnf, pt, memo);

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

  if (step <= pt_empty_index) memo[memo_index] = false;

  //if (step > pt_empty_index) {
  //  hoge++;
  //  if (hoge%1 == 0) {
  //    fprintf(stderr, "hoge %d\n", hoge);
  //    print_parse_tree(stderr, pt_empty_index, pt, bnf, token);
  //  }
  //}

  return step;
}/*}}}*/
static bool is_same_pt_exist(const int target_pt_index, const int origin_pt_index, const PARSE_TREE* pt) {/*{{{*/
  bool ret = false;
  const PARSE_TREE target = pt[target_pt_index];
  const PARSE_TREE origin = pt[origin_pt_index];

  // 呼び出し元がis_same_pt_exist(up, up)の場合は自己照合になるのでif文で弾いた
  if (target_pt_index != origin_pt_index) {
    if ( (target.bnf_id            == origin.bnf_id)
      && (target.token_begin_index == origin.token_begin_index)
      && (target.token_end_index   == origin.token_end_index)
    ) {
      ret = true;
    }
  }

  // 呼び出し元がis_same_pt_exist(up, up)の場合にも実行される
  if ((!ret) && (origin.left >= 0)) {
    if (is_same_pt_exist(target_pt_index, origin.left, pt)) ret = true;

  } else if ((!ret) && (origin.left < 0) && (origin.up >= 0)) {
    if (is_same_pt_exist(target_pt_index, origin.up, pt)) ret = true;

  }

  return ret;
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
  , bool*            memo
) {

  int step = pt_empty_index;
  for (int tmp_end=token_end_index; token_begin_index <= tmp_end; tmp_end--) {
    const int new_step = parse_match_exact(bnf_index, up_bnf_node_index, token_begin_index, tmp_end, pt_parent_index, step, token, bnf, pt, memo);
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
  , bool*            memo
) {

  int step = pt_empty_index;
  const int up = pt_parent_index;
  assert(is_syntax(bnf[pt[up].bnf_id]));

  // 親に繋がった全解析木の中で、同一syntax同一トークン列を再訪問してたら、解析せず、メモも更新しない
  if (is_same_pt_exist(up, up, pt)) return pt_empty_index;

  const MIN_REGEX_NODE* node = bnf[pt[up].bnf_id].node;

  int begin   = token_begin_index;
  int end     = token_end_index;
  int current = 0;
  int max_used_index = pt_empty_index;

  while (1) {
    if (max_used_index < step) max_used_index = step;

    // マッチング
    const int bnf_index = node_to_bnf_id(node[current], bnf);
    const int new_step  = parse_match_longest(bnf_index, current, begin, end, up, step, token, bnf, pt, memo);

    // フラグ定義/*{{{*/
    const MIN_REGEX_NODE current_node = node[current];
    const PARSE_TREE     current_pt   = pt[step];

    const bool is_match = (step < new_step);
    const bool is_end   = (current_node.is_magick) && (current_node.symbol == '$');
    const bool is_all   = (current_pt.token_begin_index == token_end_index);

    // 今回のnode達の中で、訪問済みの同一ノード同一トークン列を再訪問したかどうか
    bool is_again = false;
    int check_index = pt[pt_parent_index].down;
    if (check_index >= 0) {
      while (pt[check_index].up >= 0) {
        while (pt[check_index].right >= 0) {
          if ( (current_pt.token_begin_index == pt[check_index].token_begin_index)
            && (current_pt.token_end_index   == pt[check_index].token_end_index) // これは不必要
            && (current_pt.up_bnf_node_index == pt[check_index].up_bnf_node_index)
          ) {
            is_again = true;
            break;
          }
          if (is_again) break;
          check_index = pt[check_index].right;
        }
        check_index = pt[check_index].up;
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
static int delete_parse_tree_meta_all(const int pt_size, PARSE_TREE* pt, const BNF* bnf) {/*{{{*/
  int ret = 0;
  for (int id=0; id<pt_size; id++) {
    if (is_meta(bnf[pt[id].bnf_id])) {
      delete_parse_tree_single(id, pt);
      ret++;
    }
  }

  return ret;
}/*}}}*/
static void delete_parse_tree_single(const int id, PARSE_TREE* pt) {/*{{{*/
  // 自分を消して左右をつなぐ
  if (pt[id].left  >= 0) pt[pt[id].left].right = pt[id].right;
  if (pt[id].right >= 0) pt[pt[id].right].left = pt[id].left;

  // 自分を消して上からの矢印をずらす
  if (pt[id].up >= 0) { // 最上起点の場合を除く
    // 上からの矢印が自分を向いている場合
    if (pt[pt[id].up].down == id) {
      assert(pt[id].left == -1);
      if (pt[id].right >= 0) pt[pt[id].up].down = pt[id].right;
      else                   pt[pt[id].up].down = -1;
    }
  }
}/*}}}*/
static int bnf_token_to_memo_index(const int bnf_id, const int token_begin_index, const int token_end_index, const LEX_TOKEN* token) {/*{{{*/
  const int ts = token[0].used_size;
  return bnf_id*ts*ts + token_begin_index*ts + token_end_index;
}/*}}}*/
