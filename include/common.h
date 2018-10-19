#ifndef __MIN_BNC_PARSER_COMMON__
#define __MIN_BNC_PARSER_COMMON__

#include "../min-regex/include/min-regex.h"

typedef struct {
  int             kind;        // トークン種別
  bool            is_terminal; // BNFがLEX(終端トークン)ならtrue, SYNTAX(構文解析)ならfalse
  int             total_size;  // BNF配列全体のサイズ。charの1byte=8bit=256からメタ文字の数を引いたもの
  int             bnf_size;    // BNF配列のうち埋まったサイズ。lex_size + syntax_size
  int             lex_size;    // BNF配列のうちLEX(終端トークン)のサイズ
  int             syntax_size; // BNF配列のうちSYNTAX(構文解析BNF)のサイズ
  char            alphabet;    // kindをむりやりchar型にしたもの。syntaxの解析で一時的に使う
  const char*     bnf_str;     // bnfの文字列
  int             name_begin;  // bnf_strの左辺のトークン名の開始index
  int             name_end;    // bnf_strの左辺のトークン名の終了index
  int             def_begin;   // bnf_strの右辺のトークン名の開始index
  int             def_end;     // bnf_strの右辺のトークン名の開始index
  const char*     name;        // 左辺のトークン名の名前
  const char*     def;         // 右辺のbnfの文字列
  const char*     simple;      // defをmin-regexで解析可能にした文字列
  MIN_REGEX_NODE* node;        // simpleをmin-regexで解析したノード列
  int             node_begin;  // nodeの開始index
  int             node_end;    // nodeの終了index
} BNF;

typedef struct {
  int         id;
  int         kind;
  const char* src;
  int         begin;
  int         end;
} LEX_TOKEN;

typedef struct {
  int  id;
  int  bnf_id;
  int  bnf_node_id;
  int  token_begin_index;
  int  token_end_index;
  int  up;
  int  down;
  int  left;
  int  right;
  int  down_total;
  bool is_terminal;
} TREE;

#endif
