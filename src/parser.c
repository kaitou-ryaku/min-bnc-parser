#include "../include/common.h"
#include "../include/lexer.h"
#include "../include/text.h"
#include "../include/bnf.h"
#include "../min-regex/include/min-regex.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern int create_parser(/*{{{*/
  const char*       syntax_str
  , BNF*            bnf
  , const int       bnf_max_size
  , char*           name
  , const int       name_max_size
  , char*           def
  , const int       def_max_size
  , char*           simple
  , const int       simple_max_size
  , MIN_REGEX_NODE* node
  , const int       node_max_size
) {

  const int syntax_size = read_bnf(syntax_str, bnf, bnf_max_size, name, name_max_size, def, def_max_size);
  // int simple_seek = 0;
  // int node_seek   = 0;

  // for (int index=0; index<syntax_size; index++) {
  //   bnf[index].node    = &(node[node_seek]);
  //   bnf[index].simple  = &(simple[simple_seek]);

  //   simple_seek += simplify_regex_arbitary(
  //     bnf[index].bnf_str
  //     , bnf[index].def_begin
  //     , bnf[index].def_end
  //     , &(simple[simple_seek])
  //     , simple_max_size - simple_seek
  //   );

  //   node_seek += regex_to_all_node(
  //     bnf[index].simple
  //     , bnf[index].node
  //     , node_max_size - node_seek
  //   );
  // }

  return syntax_size;
}/*}}}*/
