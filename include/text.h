#ifndef __MIN_BNC_PARSER_TEXT__
#define __MIN_BNC_PARSER_TEXT__

#include <stdbool.h>

int  count_line_total(const char *bnf_str);
void get_next_left_index(const char* bnf_str, const int next_line, int* begin, int* end);
void get_next_right_index(const char* bnf_str, const int next_line, int* begin, int* end);
void get_next_word_index(const char* str, const int current, int* next_begin, int* next_end);
bool is_same_word(const char* str1, const int begin1, const int end1, const char* str2, const int begin2, const int end2);

#endif
