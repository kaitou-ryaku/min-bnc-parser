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
static int parse_match_exact(
  const   int        bnf_index
  , const int        token_begin_index
  , const int        token_end_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
  , bool*            memo
);
static int parse_match_shortest(
  const   int        bnf_index
  , const int        token_begin_index
  , const int        token_end_index
  , const int        token_final_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
  , bool*            memo
);
static int parse_syntax_recursive(
  const   int        bnf_index
  , const int        token_begin_index
  , const int        token_end_index
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
static void back_track_update_index(
  const   int   bnf_index
  , const int   right_side_before_match
  , const int   token_end_index
  , int*        current_pt_index
  , int*        current_token_begin_index
  , int*        current_token_end_index
  , int*        current_node_index
  , const BNF*  bnf
  , PARSE_TREE* pt
);
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

  const int memo_size = 255*(token[0].used_size+1)*(token[0].used_size+1);
  assert(memo_size < memo_max_size);
  for (int i=0; i<memo_max_size; i++) memo[i] = true;

  initialize_parse_tree(pt, pt_max_size);

  const int begin_bnf_index = search_bnf_next_syntax(-1, bnf);

  int step = parse_match_exact(begin_bnf_index, 0, token[0].used_size, 0, token, bnf, pt, memo);

  fprintf(stderr, "TOTAL PARSE TREE STEP:%d\n", step);
  print_parse_tree(stderr, step, pt, bnf, token);
  delete_parse_tree_meta_all(step, pt, bnf);

  return step;
}/*}}}*/
static int parse_match_exact(/*{{{*/
  const   int        bnf_index
  , const int        token_begin_index
  , const int        token_end_index
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

  // stepを孤立ノードにする
  initialize_parse_tree_unit(pt, step);
  pt[step].bnf_id            = bnf_index;
  pt[step].token_begin_index = token_begin_index;
  pt[step].token_end_index   = token_end_index;

  // 受け取ったBNFがMETAの場合
  if ( (is_meta(bnf[bnf_index]))
    && (token_begin_index == token_end_index)
  ) {

    pt[step].state = 1;
    step++;
  }

  // 受け取ったBNFがLEXの場合
  else if ( (is_lex(bnf[bnf_index]))
    && (token_begin_index + 1 == token_end_index)
    && (bnf[bnf_index].kind == token[token_begin_index].kind)
  ) {

    pt[step].state = 2;
    step++;
  }

  // 受け取ったBNFがSYNTAXの場合
  else if (is_syntax(bnf[bnf_index])) {

    pt[step].state = 3;

    // TODO 下のノードを再帰的に解析
    const int new_step = parse_syntax_recursive(bnf_index, token_begin_index, token_end_index, step+1, token, bnf, pt, memo);

    if (step+1 < new_step) {
      // 再帰的構文解析に成功すれば、上を下の左端と接続
      pt[step].down = step+1;

      // 作成された下の全ノードを上に接続
      int tmp_step = step+1;
      while (tmp_step >= 0) {
        pt[tmp_step].up = step;
        tmp_step = pt[tmp_step].right;
      }

      step = new_step;
    }
  }

  // TODO
  // 他が解析中で無限ループ処理でsyntax_recursiveが止まったときもfalseになるからヤバイ？
  if (step <= pt_empty_index) memo[memo_index] = false;

  //if (step > pt_empty_index) {
  //  hoge++;
  //  if (hoge%1000 == 0) {
  //    fprintf(stderr, "hoge %d\n", hoge);
  //    print_parse_tree(stderr, pt_empty_index, pt, bnf, token);
  //  }
  //}

  return step;
}/*}}}*/
static int parse_match_shortest(/*{{{*/
  const   int        bnf_index
  , const int        token_begin_index
  , const int        token_end_index
  , const int        token_final_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
  , bool*            memo
) {

  int step = pt_empty_index;
  for (int tmp_end=token_end_index; tmp_end <= token_final_index; tmp_end++) {
    const int new_step = parse_match_exact(bnf_index, token_begin_index, tmp_end, step, token, bnf, pt, memo);
    if (step < new_step) {
      step = new_step;
      break;
    }
  }

  return step;
}/*}}}*/
static int parse_syntax_recursive(/*{{{*/
  const   int        bnf_index
  , const int        token_begin_index
  , const int        token_end_index
  , const int        pt_empty_index
  , const LEX_TOKEN* token
  , const BNF*       bnf
  , PARSE_TREE*      pt
  , bool*            memo
) {

  assert(is_syntax(bnf[bnf_index]));

  // 親に繋がった全解析木の中で、同一syntax同一トークン列を再訪問してたら、解析せず、メモも更新しない
  // この処理は左再帰の無限ループ対策だが、メモ化が難しくなるので、入れないことにする。BNFの左再帰は禁止で
  // if (is_same_pt_exist(up, up, pt)) return pt_empty_index;

  const MIN_REGEX_NODE* node = bnf[bnf_index].node;

  const int left_side = pt_empty_index;
  int current_token_begin_index = token_begin_index;
  int current_token_end_index   = token_begin_index;

  int current_node_index = 0;
  int current_pt_index = parse_match_shortest(
    node_to_bnf_id(node[current_node_index], bnf)
    , current_token_begin_index
    , current_token_end_index
    , token_end_index
    , left_side
    , token
    , bnf
    , pt
    , memo
  );

  assert(left_side+1 == current_pt_index);
  pt[left_side].up_bnf_node_index = current_node_index;
  current_node_index = node[current_node_index].out_fst;

  while (1) {

    // マッチングの前の右端ノードを取得/*{{{*/
    int right_side_before_match = left_side;
    while (pt[right_side_before_match].right >= 0) {
      right_side_before_match = pt[right_side_before_match].right;
    }/*}}}*/

    // マッチング/*{{{*/
    const int current_bnf_index = node_to_bnf_id(node[current_node_index], bnf);
    const int next_pt_index = parse_match_shortest(
      current_bnf_index
      , current_token_begin_index
      , current_token_end_index
      , token_end_index
      , current_pt_index
      , token
      , bnf
      , pt
      , memo
    );
    if (current_pt_index < next_pt_index) pt[current_pt_index].up_bnf_node_index = current_node_index;/*}}}*/

    // フラグ定義/*{{{*/
    const bool is_match = (current_pt_index < next_pt_index);

    const MIN_REGEX_NODE current_node = node[current_node_index];
    const bool is_end = (current_node.is_magick) && (current_node.symbol == '$');

    const PARSE_TREE current_pt = pt[current_pt_index];
    const bool is_end_with_empty = (current_pt.token_begin_index == token_end_index);

    // 今回のnode達の中で、訪問済みの同一ノード同一トークン列を再訪問したかどうか
    bool is_again = false;
    int check = left_side;
    while (check >= 0) {
      if ( (current_pt.token_begin_index == pt[check].token_begin_index)
        && (current_pt.token_end_index   == pt[check].token_end_index) // これは不必要
        && (current_pt.up_bnf_node_index == pt[check].up_bnf_node_index)
      ) {
        is_again = true;
        break;
      }
      check = pt[check].right;
    }/*}}}*/

    // マッチ成功かつ$でない -> out_fstノードへ移動/*{{{*/
    if (is_match && (!is_again) && (!is_end)) {

      // 新規作成されたノードを左ノードにつなぐ
      pt[right_side_before_match].right = current_pt_index;
      pt[current_pt_index].left = right_side_before_match;

      // トークンを更新して次のノードに移動
      current_token_begin_index = pt[current_pt_index].token_end_index;
      current_token_end_index   = current_token_begin_index;
      current_node_index = node[current_node_index].out_fst;
      current_pt_index = next_pt_index;
    }/*}}}*/

    // マッチ成功かつ$で、ちゃんとトークン列が終了している -> 終了/*{{{*/
    else if (is_match && is_end && is_end_with_empty) {
      assert(!is_again);

      // 新規作成されたノードを左ノードにつなぐ
      pt[right_side_before_match].right = current_pt_index;
      pt[current_pt_index].left = right_side_before_match;

      // $ノードでトークンを残さずマッチに成功した場合は、ループを抜ける
      current_pt_index = next_pt_index;
      break;
    }/*}}}*/

    // マッチ失敗 ($ノードで、トークンを残したままマッチした場合を含む) -> バックトラック/*{{{*/
    else {

      back_track_update_index(
        bnf_index
        , token_end_index
        , right_side_before_match
        , &current_pt_index
        , &current_token_begin_index
        , &current_token_end_index
        , &current_node_index
        , bnf
        , pt
      );

      // ^ノードに到達したら、マッチ失敗として終了
      if (current_pt_index < 0) {
        current_pt_index = pt_empty_index;
        break;
      }
    }/*}}}*/
  }

  return current_pt_index;
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
  const int ts = token[0].used_size+1;
  return bnf_id*ts*ts + token_begin_index*ts + token_end_index;
}/*}}}*/
static void back_track_update_index(/*{{{*/
  const   int   bnf_index
  , const int   token_end_index
  , const int   right_side_before_match
  , int*        current_pt_index
  , int*        current_token_begin_index
  , int*        current_token_end_index
  , int*        current_node_index
  , const BNF*  bnf
  , PARSE_TREE* pt
) {

  const MIN_REGEX_NODE* node = bnf[bnf_index].node;
  int pt_index = right_side_before_match;
  int right_node_index = (*current_node_index);

  while (1) {
    const PARSE_TREE target_pt = pt[pt_index];
    const MIN_REGEX_NODE target_node = node[target_pt.up_bnf_node_index];

    // バックトラック対象が分岐ノード
    if (target_node.out_fst == right_node_index && target_node.out_snd >= 0) {
      pt[pt_index].right           = -1; // 分岐発生ノードの以前の接続情報を削除
      (*current_node_index)        = target_node.out_snd;
      (*current_token_begin_index) = target_pt.token_end_index;
      (*current_token_end_index)   = target_pt.token_end_index;
      //TODO
      break;

    // 最初の^ノードに到達した場合、終了
    } else if (target_pt.left < 0) {
      assert(node[target_pt.up_bnf_node_index].symbol == '^');
      pt[pt_index].right           = -1; // 分岐発生ノードの以前の接続情報を削除
      (*current_node_index)        = -1;
      (*current_token_begin_index) = -1;
      (*current_token_end_index)   = -1;
      (*current_pt_index)          = -1;
      break;

    // 分岐ではなく、トークンが残っていれば、そのノードを再探索
    } else if (target_pt.token_end_index < token_end_index) {
      pt[pt[pt_index].left].right  = -1; // 分岐発生ノードの以前の接続情報を削除
      (*current_node_index)        = target_pt.up_bnf_node_index;
      (*current_token_begin_index) = target_pt.token_begin_index;
      (*current_token_end_index)   = target_pt.token_end_index+1;
      (*current_pt_index)          = right_side_before_match;
      break;

    // さらにバックトラック
    } else {
      right_node_index = target_pt.up_bnf_node_index;
      pt_index = target_pt.left;
    }
  }
}/*}}}*/
