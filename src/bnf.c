#include "../include/common.h"
#include "../include/text.h"
#include "../min-regex/include/min-regex.h"
#include <assert.h>

static int search_bnf_unused(const BNF* bnf);
static int search_bnf_next_state(const int bnf_state, const int current_bnf_index, const BNF* bnf);

extern void initialize_bnf(/*{{{*/
  BNF* bnf
  , const int bnf_max_size
) {

  assert(bnf_max_size == 255);
  char alphabet = -127;
  const char meta[]   = {'(', '|', ')', '*', '@', '.', '^', '$', '\\', '"', '\0'};
  const int meta_size = sizeof(meta)/sizeof(char);

  for (int i=0; i<bnf_max_size; i++) {

    bnf[i].kind        = i;            // 配列の添字
    bnf[i].state       = 0;            // 非使用なら0, メタ文字なら1, LEXなら2, SYNTAXなら3
    bnf[i].total_size  = bnf_max_size; // BNF配列のサイズ
    bnf[i].alphabet    = alphabet;     // kindをむりやりchar型にしたもの。syntaxの解析で一時的に使う

    bnf[i].bnf_str     = NULL;         // bnfの文字列
    bnf[i].name_begin  = -1;           // bnf_strの左辺のトークン名の開始index
    bnf[i].name_end    = -1;           // bnf_strの左辺のトークン名の終了index
    bnf[i].def_begin   = -1;           // bnf_strの右辺のトークン名の開始index
    bnf[i].def_end     = -1;           // bnf_strの右辺のトークン名の開始index

    bnf[i].name        = NULL;         // 左辺のトークン名の名前
    bnf[i].def         = NULL;         // 右辺のbnfの文字列
    bnf[i].simple      = NULL;         // defをmin-regexで解析可能にした文字列
    bnf[i].node        = NULL;         // simpleをmin-regexで解析したノード列の開始ポインタ
    bnf[i].node_size   = 0;            // ノード列のサイズ

    for (int j=0; j<meta_size; j++) {
      if (alphabet == meta[j]) bnf[i].state = 1;
    }

    alphabet++;
  }
}/*}}}*/
extern int read_bnf(/*{{{*/
  const char* bnf_str
  , BNF*      bnf
  , char*     name
  , const int name_max_size
  , char*     def
  , const int def_max_size
  , const int meta_lex_syntax_state
) {

  const int line_total = count_line_total(bnf_str);
  assert(line_total < bnf[0].total_size);

  int name_seek   = 0;
  int def_seek    = 0;

  for (int line=0; line<line_total; line++) {
    const int index = search_bnf_unused(bnf);
    bnf[index].state   = meta_lex_syntax_state;

    bnf[index].bnf_str = bnf_str;
    bnf[index].name    = &(name[name_seek]);
    bnf[index].def     = &(def[def_seek]);

    int begin, end;
    get_next_left_index(bnf_str, line, &begin, &end);
    bnf[index].name_begin = begin;
    bnf[index].name_end   = end;
    assert(name_seek + end - begin + 1 < name_max_size);

    for (int seek=0; seek<end-begin; seek++) {
      name[name_seek + seek] = bnf_str[begin + seek];
    }
    name[name_seek + end - begin] = '\0';
    name_seek += end - begin + 1;

    get_next_right_index(bnf_str, line, &begin, &end);
    bnf[index].def_begin = begin;
    bnf[index].def_end   = end;
    assert(def_seek + end - begin + 1 < def_max_size);

    for (int seek=0; seek<end-begin; seek++) {
      def[def_seek + seek] = bnf_str[begin + seek];
    }
    def[def_seek + end - begin] = '\0';
    def_seek += end - begin + 1;
  }

  return line_total;
}/*}}}*/
static int search_bnf_unused(const BNF* bnf) {/*{{{*/
  const int bnf_total_size = bnf[0].total_size;

  int i;
  for (i=0; i<bnf_total_size; i++) {
    if (bnf[i].state == 0) {
      break;
    }
  }

  if (i == bnf_total_size) {
    fprintf(stderr, "ERROR: BNF ARRAY IS FULL\n");
    assert(0);
  }

  return i;
}/*}}}*/
static int search_bnf_next_state(const int bnf_state, const int current_bnf_index, const BNF* bnf) {/*{{{*/
  const int bnf_total_size = bnf[0].total_size;

  int i;
  for (i = current_bnf_index+1; i<bnf_total_size; i++) {
    if (i < 0) continue;

    if (bnf[i].state == bnf_state) {
      break;
    }
  }

  if (i == bnf_total_size) i = -1;

  return i;
}/*}}}*/
extern int search_bnf_next_lex(const int current_bnf_index, const BNF* bnf) {/*{{{*/
  return search_bnf_next_state(2, current_bnf_index, bnf);
}/*}}}*/
extern int search_bnf_next_syntax(const int current_bnf_index, const BNF* bnf) {/*{{{*/
  return search_bnf_next_state(3, current_bnf_index, bnf);
}/*}}}*/
extern int search_bnf_next_lex_or_syntax(const int current_bnf_index, const BNF* bnf) {/*{{{*/
  const int lex    = search_bnf_next_state(2, current_bnf_index, bnf);
  const int syntax = search_bnf_next_state(3, current_bnf_index, bnf);
  if      (lex    < 0)    return syntax;
  else if (syntax < 0)    return lex;
  else if (lex < syntax ) return lex;
  else                    return syntax;
}/*}}}*/
extern bool is_meta(const BNF bnf) {/*{{{*/
  if (bnf.state == 1) return true;
  else return false;
}/*}}}*/
extern bool is_lex(const BNF bnf) {/*{{{*/
  if (bnf.state == 2) return true;
  else return false;
}/*}}}*/
extern bool is_syntax(const BNF bnf) {/*{{{*/
  if (bnf.state == 3) return true;
  else return false;
}/*}}}*/
extern bool is_lex_or_syntax(const BNF bnf) {/*{{{*/
  if ((bnf.state == 2) || (bnf.state == 3)) return true;
  else return false;
}/*}}}*/
extern BNF node_to_bnf(const MIN_REGEX_NODE node, const BNF* bnf) {/*{{{*/
  return bnf[node.symbol + 127];
}/*}}}*/
