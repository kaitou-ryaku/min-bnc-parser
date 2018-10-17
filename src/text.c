#include "../include/text.h"

extern int count_line_total(const char *bnf_str) {/*{{{*/
  int line = 0;
  int seek = 0;
  while (bnf_str[seek] != '\0') {
    if (bnf_str[seek] == '\n') line++;
    seek++;
  }
  return line;
}/*}}}*/
extern void get_next_left_index(const char* bnf_str, const int next_line, int* begin, int* end) {/*{{{*/
  int seek = 0;
  int line = 0;
  while (line < next_line) {
    if (bnf_str[seek] == '\n') line++;
    seek++;
  }

  while (bnf_str[seek] == ' ') seek++;
  (*begin) = seek;

  while (bnf_str[seek] != ' ') seek++;
  (*end) = seek;
}/*}}}*/
extern void get_next_right_index(const char* bnf_str, const int next_line, int* begin, int* end) {/*{{{*/
  int seek = 0;
  int line = 0;
  while (line < next_line) {
    if (bnf_str[seek] == '\n') line++;
    seek++;
  }

  while (bnf_str[seek] != ':') seek++;
  seek++;

  while (bnf_str[seek] == ' ') seek++;
  (*begin) = seek;

  while (bnf_str[seek] != '\n') seek++;
  (*end) = seek;
}/*}}}*/
