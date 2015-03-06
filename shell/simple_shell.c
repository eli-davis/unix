#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "shell_library.h"

void print_commands()
{ printf("\nCommands: \n");
  printf("  help \n");
  printf("  echo \n");
  printf("  q/quit/exit/logout \n");
  printf("  run executable_name arg1 arg2 arg3 \n\n"); }

int quit_command(char * argument)
{ int quit = 0;
  if ( !strcmp(argument, "q") || !strcmp(argument, "quit") ||
       !strcmp(argument, "exit") || !strcmp(argument, "logout") )
    quit = 1;
  return quit; }

void run_program(char * * s, int n_args)
{ int i, pid, pid2, status;
  char * args[max_arg_c];
  for (i = 0; i < n_args; i += 1)
    args[i] = strdup(s[i+1]);
  args[i-1] = NULL;
  pid = fork();
  if (pid == 0)
  { int r = execvp(args[0], args);
    printf("program not found. ");
    exit(1); }
  pid2 = wait(& status); }

void simple_shell()
{ int n_args, i;
  char * * sarray = malloc(max_arg_c * sizeof(*sarray));
  for (i = 0; i < max_arg_c; i += 1)
    sarray[i] = malloc(max_arg_length + 1);
  print_commands();
  while (1)
  { printf("\n---> ");
    for (i = 0; i < max_arg_c; i += 1)
      *sarray[i] = 0;
    n_args = get_command_line(sarray);
    if (quit_command(sarray[0]))
    { printf("exiting shell \n");
      for (i = 0; i < max_arg_c; i += 1)
        free(sarray[i]);
      free(sarray);
      break; }
    else if (!strcmp(sarray[0], "help"))
      print_commands();
    else if (!strcmp(sarray[0], "echo"))
    { for (i = 1; i < n_args && sarray[i] != NULL; i += 1)
        printf("%s ", sarray[i]); }
    else if (!strcmp(sarray[0], "run"))
      run_program(sarray, n_args);
    else
      printf("command not found. "); } }

int main()
{ simple_shell(); }
