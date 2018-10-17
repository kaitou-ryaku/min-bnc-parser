#ifndef __MIN_BNC_PARSER_TEXT__
#define __MIN_BNC_PARSER_TEXT__

int  count_line_total(const char *bnf_str);
void get_next_left_index(const char* bnf_str, const int next_line, int* begin, int* end);
void get_next_right_index(const char* bnf_str, const int next_line, int* begin, int* end);

#endif
