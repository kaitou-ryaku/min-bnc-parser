#include "../include/common.h"
#include "../include/text.h"
#include "../min-regex/include/min-regex.h"
#include <assert.h>
extern void initialize_bnf(/*{{{*/
  BNF* bnf
  , const int bnf_max_size
) {

  const int except_alphabet_size = 11;
  assert(bnf_max_size + except_alphabet_size == 255);

  char alphabet = -127;
  for (int i=0; i<bnf_max_size; i++) {
    for (int j=0; j<except_alphabet_size; j++) {
      if ( alphabet == '('
        || alphabet == '|'
        || alphabet == ')'
        || alphabet == '*'
        || alphabet == '@'
        || alphabet == '.'
        || alphabet == '^'
        || alphabet == '$'
        || alphabet == '\\'
        || alphabet == '"'
        || alphabet == '\0'
      ) alphabet++;
    }

    bnf[i].kind        = i;            // kind       トークン種別
    bnf[i].is_terminal = false;        // BNFがLEX(終端トークン)ならtrue, SYNTAX(構文解析)ならfalse
    bnf[i].total_size  = bnf_max_size; // BNF配列のサイズ
    bnf[i].bnf_size    = 0;            // BNF配列のサイズ
    bnf[i].lex_size    = 0;            // BNF配列のうちLEX(終端トークン)のサイズ
    bnf[i].syntax_size = 0;            // BNF配列のうちSYNTAX(構文解析BNF)のサイズ
    bnf[i].alphabet    = alphabet;     // alphabet   kindをむりやりchar型にしたもの。syntaxの解析で一時的に使う
    bnf[i].bnf_str     = NULL;         // str        bnfの文字列
    bnf[i].name_begin  = -2;           // name_begin bnf_strの左辺のトークン名の開始index
    bnf[i].name_end    = -2;           // name_end   bnf_strの左辺のトークン名の終了index
    bnf[i].def_begin   = -2;           // bnf_begin  bnf_strの右辺のトークン名の開始index
    bnf[i].def_end     = -2;           // bnf_end    bnf_strの右辺のトークン名の開始index
    bnf[i].name        = NULL;         // name       左辺のトークン名の名前
    bnf[i].def         = NULL;         // bnf        右辺のbnfの文字列
    bnf[i].simple      = NULL;         // simple     defをmin-regexで解析可能にした文字列
    bnf[i].node        = NULL;         // node       simpleをmin-regexで解析したノード列
    bnf[i].node_begin  = -2;           // node_begin nodeの開始index
    bnf[i].node_end    = -2;           // node_end   nodeの終了index
    alphabet++;
  }
}/*}}}*/
extern int read_bnf(/*{{{*/
  const char*       bnf_str
  , BNF*            bnf
  , const int       bnf_max_size
  , char*           name
  , const int       name_max_size
  , char*           def
  , const int       def_max_size
) {

  int name_seek   = 0;
  int def_seek    = 0;

  const int line_total = count_line_total(bnf_str);
  assert(line_total < bnf_max_size);
  for (int line=0; line<line_total; line++) {
    bnf[line].bnf_str = bnf_str;
    bnf[line].name    = &(name[name_seek]);
    bnf[line].def     = &(def[def_seek]);

    int begin, end;
    get_next_left_index(bnf_str, line, &begin, &end);
    bnf[line].name_begin = begin;
    bnf[line].name_end   = end;
    assert(name_seek + end - begin + 1 < name_max_size);

    for (int seek=0; seek<end-begin; seek++) {
      name[name_seek + seek] = bnf_str[begin + seek];
    }
    name[name_seek + end - begin] = '\0';
    name_seek += end - begin + 1;

    get_next_right_index(bnf_str, line, &begin, &end);
    bnf[line].def_begin = begin;
    bnf[line].def_end   = end;
    assert(def_seek + end - begin + 1 < def_max_size);

    for (int seek=0; seek<end-begin; seek++) {
      def[def_seek + seek] = bnf_str[begin + seek];
    }
    def[def_seek + end - begin] = '\0';
    def_seek += end - begin + 1;
  }

  return line_total;
}/*}}}*/
