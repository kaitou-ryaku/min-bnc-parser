#include "../include/lexer.h"
#include <stdio.h>
#include <assert.h>

int main(int argc, const char **argv) {
  if (argc != 1) {
    fprintf(stderr, "Invalid argc (%d)\n", argc);
    return 1;
  }

  lexer("(11+20)*201-(21-1)/(0-1*a0b+b)");
}
