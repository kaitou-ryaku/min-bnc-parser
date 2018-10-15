#include "../include/lexer.h"
#include <stdio.h>
#include <assert.h>

int main(int argc, const char **argv) {
  if (argc != 1) {
    fprintf(stderr, "Invalid argc (%d)\n", argc);
    return 1;
  }

  lexer("hoge");
}
