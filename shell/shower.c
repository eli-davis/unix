#include <stdio.h>

int main(int argc, char * argv[])
{ int i;
  printf("started, argc = %d\n", argc);
  sleep(1);
  for (i = 0; i <= argc; i += 1)
    printf("argv[%d] = \"%s\"\n", i, argv[i]);
  printf("stopped\n");
  return 0; }

