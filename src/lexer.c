#include "../include/lexer.h"
#include "../min-regex/include/min-regex.h"
#include <stdio.h>
#include <string.h>

extern void lexer(const char *regex_str, const char *match_str) {
  char simple_regex[200];
  simplify_regex(regex_str, simple_regex, 200);
  MIN_REGEX_NODE node[100];
  regex_to_all_node(simple_regex, node, 100);
  MIN_REGEX_MATCH match[50];
  // const int step = exact_match(match_str, node, match, 50);
  const int index = forward_shortest_match( match_str, node, match, 50);

  for (int i=0; i<index; i++) fprintf(stderr, "%c", match_str[i]);
  fprintf(stderr, "\n");
}
