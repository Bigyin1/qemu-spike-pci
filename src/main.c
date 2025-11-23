#include "spike-bind.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  FILE *elf = fopen("my_program.elf", "rb");

  fseek(elf, 0, SEEK_END);
  long size = ftell(elf);
  fseek(elf, 0, SEEK_SET);

  char *fcontent = (char *)malloc(size);
  fread(fcontent, 1, size, elf);
  fclose(elf);


  printf("%ld\n", spike_run(fcontent, size));
  return 0;
}
