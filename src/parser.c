#include "../include/common.h"
#include "../include/lexer.h"
#include "../include/text.h"
#include "../include/bnf.h"
#include "../min-regex/include/min-regex.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern int create_parser(/*{{{*/
  const char*       syntax_str
  , const int       lex_size
  , char*           work
  , const int       work_max_size
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

  const int syntax_size = read_bnf(syntax_str, &(bnf[lex_size]), bnf_max_size, name, name_max_size, def, def_max_size);
  const int bnf_size    = lex_size + syntax_size;

  int simple_seek = 0;
  int node_seek   = 0;

  for (int index=lex_size; index<bnf_size; index++) {
    bnf[index].node    = &(node[node_seek]);
    bnf[index].simple  = &(simple[simple_seek]);

    int seek      = 0;
    int work_seek = 0;
    const int def_size = strlen(bnf[index].def);
    while (seek < def_size) {
      int begin, end;
      get_next_word_index(bnf[index].def, seek, &begin, &end);
      const char c = (bnf[index].def)[begin];

      if ((c == '(') || (c == '|') || (c == ')') || (c == '*') || (c == '@')) work[work_seek] = c;
      else {
        int cmp_index;
        for (cmp_index=0; cmp_index<bnf_size; cmp_index++) {
          const char* name1 = bnf[index].def;
          const char* name2 = bnf[cmp_index].name;
          if (is_same_word(name1, begin, end, name2, 0, strlen(name2))) {
            // work[work_seek] = bnf[index].alphabet;
            work[work_seek] = 'a';
            break;
          }
        }

        if (cmp_index >= bnf_size) {
          fprintf(stderr, "ERROR: NO TOKEN OR ATTRIBUTE EXISTS IN LEFT SIDE OF BNF ... ");
          for (int i=begin; i<end; i++) fprintf(stderr, "%c", (bnf[index].def)[i]);
          fprintf(stderr, "\n");
          assert(cmp_index < bnf_size);
        }
      }

      // for (int i=begin; i<end; i++) fprintf(stderr, "%c", (bnf[index].def)[i]);
      // fprintf(stderr, "\n");
      seek = end;
      work_seek++;
    }
    work[work_seek] = '\0';

    //fprintf(stderr, "length:%d\n", (int)strlen(work));
    //fprintf(stderr, "work:%s\n", work);

    //simple_seek += simplify_regex_arbitary(
    //  bnf[index].bnf_str
    //  , bnf[index].def_begin
    //  , bnf[index].def_end
    //  , &(simple[simple_seek])
    //  , simple_max_size - simple_seek
    //);

    //node_seek += regex_to_all_node(
    //  bnf[index].simple
    //  , bnf[index].node
    //  , node_max_size - node_seek
    //);
  }

  for (int index=lex_size; index<bnf_size; index++) {
    fprintf(stderr, "index: %02d | ", index);
    fprintf(stderr, "name: %15s  | ", bnf[index].name);
    fprintf(stderr, "def: %s ", bnf[index].def);
    fprintf(stderr, "\n");
  }

  return syntax_size;
}/*}}}*/
