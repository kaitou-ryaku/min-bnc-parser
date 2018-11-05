#include "../include/common.h"
#include "../include/memo.h"
#include <stdbool.h>

// 関数プロトタイプ/*{{{*/
static int search_unused_memo_index(const char bnf_id, const int token_begin_index, const int token_end_index, const PARSE_MEMO* memo);
static int search_registered_memo_index(const char bnf_id, const int token_begin_index, const int token_end_index, const PARSE_MEMO* memo);
static int get_raw_hash(const char bnf_id, const int token_begin_index, const int token_end_index, const int hash_max);
/*}}}*/
extern void initialize_memo(PARSE_MEMO* memo, const int memo_max_size) {/*{{{*/
  for (int i=0; i<memo_max_size; i++) {
    memo[i].state  = true;
    memo[i].bnf_id = 0;
    memo[i].token_begin_index = -1;
    memo[i].token_end_index   = -1;
    memo[i].total_size = memo_max_size;
  }
}/*}}}*/
extern int register_memo(const bool state, const char bnf_id, const int token_begin_index, const int token_end_index, PARSE_MEMO* memo) {/*{{{*/
  const int hash = search_unused_memo_index(bnf_id, token_begin_index, token_end_index, memo);
  if (hash >= 0) {
    memo[hash].state = state;
    memo[hash].bnf_id = bnf_id;
    memo[hash].token_begin_index = token_begin_index;
    memo[hash].token_end_index = token_end_index;
  }
  return hash;
}/*}}}*/
extern bool search_memo(const char bnf_id, const int token_begin_index, const int token_end_index, const PARSE_MEMO* memo) {/*{{{*/
  const int hash = search_registered_memo_index(bnf_id, token_begin_index, token_end_index, memo);

  bool ret;
  if (hash == -1) {
    ret = true;
  } else {
    ret = memo[hash].state;
  }

  return ret;
}/*}}}*/
static int search_unused_memo_index(const char bnf_id, const int token_begin_index, const int token_end_index, const PARSE_MEMO* memo) {/*{{{*/
  const int hash_max = memo[0].total_size;
  int hash = get_raw_hash(bnf_id, token_begin_index, token_end_index, hash_max);

  const int end = (hash-1+hash_max) % hash_max;
  while (end != hash) {
    if (memo[hash].token_begin_index == -1) break;
    else hash = (hash+1) % hash_max;
  }

  if (hash == end) hash = -1;
  return hash;
}/*}}}*/
static int search_registered_memo_index(const char bnf_id, const int token_begin_index, const int token_end_index, const PARSE_MEMO* memo) {/*{{{*/
  const int hash_max = memo[0].total_size;
  int hash = get_raw_hash(bnf_id, token_begin_index, token_end_index, hash_max);

  const int end = (hash-1+hash_max) % hash_max;
  while (end != hash) {
    if ( (memo[hash].bnf_id            == bnf_id)
      && (memo[hash].token_begin_index == token_begin_index)
      && (memo[hash].token_end_index   == token_end_index)
    ) {
      break;
    } else if (memo[hash].token_begin_index == -1) {
      hash = -1;
      break;
    }
    else hash = (hash+1) % hash_max;
  }
  if (hash == end) hash = -1;

  return hash;
}/*}}}*/
static int get_raw_hash(const char bnf_id, const int token_begin_index, const int token_end_index, const int hash_max) {/*{{{*/
  const int magick = 997;
  const int delta = 313;

  int hash;
  hash = (bnf_id+256) * magick + delta;
  hash = hash % hash_max;
  hash = hash * magick + token_begin_index * delta;
  hash = hash % hash_max;
  hash = hash * magick + token_end_index * delta;
  hash = hash % hash_max;
  return hash;
}/*}}}*/
