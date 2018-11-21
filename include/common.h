#ifndef __MIN_BNC_PARSER_COMMON__
#define __MIN_BNC_PARSER_COMMON__

#include "../min-regex/include/min-regex.h"

typedef struct {
  int             kind;        // 配列の添字番号
  int             state;       // 非使用なら0, メタ文字なら1, LEXなら2, SYNTAXなら3
  int             total_size;  // char型の255を想定
  int             lex_size;    // BNF配列のうちLEX(終端トークン)のサイズ
  int             syntax_size; // BNF配列のうちSYNTAX(構文解析BNF)のサイズ
  int             meta_size;   // BNF配列のうちメタ文字のサイズ
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
  int             node_size;   // nodeの終了index+1
} BNF;

typedef struct {
  int         id;
  int         total_size;
  int         used_size;
  int         kind;
  const char* src;
  int         begin;
  int         end;
  char*       name;
} LEX_TOKEN;

typedef struct {
  int  id;
  int  state;
  int  total_size;
  int  used_size;
  int  bnf_id;
  int  up_bnf_node_index;
  int  token_begin_index;
  int  token_end_index;
  int  up;
  int  down;
  int  left;
  int  right;
} PARSE_TREE;

typedef struct {
  int  id;
  int  total_size;
  int  used_size;
  int  left_bnf_id;
  int  right_bnf_id;
} PAIR_BNF;

typedef struct {
  bool state;
  char bnf_id;
  int token_begin_index;
  int token_end_index;
  int total_size;
} PARSE_MEMO;

static const int PT_STATE_UNUSED = 0;
static const int PT_STATE_META   = 1;
static const int PT_STATE_LEX    = 2;
static const int PT_STATE_SYNTAX = 3;

#endif
