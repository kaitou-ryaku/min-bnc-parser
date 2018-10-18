#ifndef __MIN_BNC_PARSER_COMMON__
#define __MIN_BNC_PARSER_COMMON__

#include "../min-regex/include/min-regex.h"

typedef struct {
  int             kind;       // トークン種別
  char            alphabet;   // kindをむりやりchar型にしたもの。syntaxの解析で一時的に使う
  const char*     bnf_str;    // bnfの文字列
  int             name_begin; // bnf_strの左辺のトークン名の開始index
  int             name_end;   // bnf_strの左辺のトークン名の終了index
  int             def_begin;  // bnf_strの右辺のトークン名の開始index
  int             def_end;    // bnf_strの右辺のトークン名の開始index
  const char*     name;       // 左辺のトークン名の名前
  const char*     def;        // 右辺のbnfの文字列
  const char*     simple;     // defをmin-regexで解析可能にした文字列
  MIN_REGEX_NODE* node;       // simpleをmin-regexで解析したノード列
  int             node_begin; // nodeの開始index
  int             node_end;   // nodeの終了index
} BNF;

#endif
