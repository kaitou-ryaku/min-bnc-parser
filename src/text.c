#include "../include/text.h"
#include <ctype.h>
#include <stdbool.h>

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
extern void get_next_word_index(const char* str, const int current, int* next_begin, int* next_end) {/*{{{*/
  int seek = current;

  while ((str[seek] == ' ')) seek++;
  (*next_begin) = seek;

  seek++;

  while ((isalpha(str[seek])) || (str[seek] == '_') || (str[seek] == '-')) seek++;
  (*next_end) = seek;
}/*}}}*/
extern bool is_same_word(const char* str1, const int begin1, const int end1, const char* str2, const int begin2, const int end2) {/*{{{*/
  bool ret = true;
  if (begin1 - end1 != begin2 - end2) {
    ret = false;
  } else {
    for (int i=0; i<end1-begin1; i++) {
      if (str1[begin1+i] != str2[begin2+i]) {
        ret = false;
        break;
      }
    }
  }
  return ret;
}/*}}}*/
