#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "shell_library.h"

typedef int bool;

void print_user_commands()
{ printf("\nCommands: \n");
  printf("  help \n");
  printf("  echo \n");
  printf("  q/quit/exit/logout \n");
  printf("  run executable_name arg1 arg2 arg3 \n\n"); }

bool quit_command(char * argument)
{ int quit = 0;
  if ( !strcmp(argument, "q") || !strcmp(argument, "quit") ||
       !strcmp(argument, "exit") || !strcmp(argument, "logout") )
    quit = 1;
  return quit; }

void run_program(char * * s, int argc)
{ int i, pid, pid2, status;
  char * args[max_argc];
  for (i = 0; i < argc; i += 1)
    args[i] = strdup(s[i+1]);
  args[i-1] = NULL;
  pid = fork();
  if (pid == 0)
  { int r = execvp(args[0], args);
    printf("program not found. ");
    exit(1); }
  pid2 = wait(& status); }

void exit_shell(char * * string_array)
{ int i;
  printf("exiting shell \n");
  for (i = 0; i < max_argc; i += 1)
    free(string_array[i]);
  free(string_array); }

void echo_commands(char * * string_array, int argc)
{ int i;
  for (i = 1; i < argc && string_array[i]; i += 1)
  printf("%s ", string_array[i]); }

void simple_shell()
{ int i, argc;
  char * * string_array = malloc(max_argc * sizeof(*string_array));
  for (i = 0; i < max_argc; i += 1)
    string_array[i] = malloc(max_arg_length + 1);
  print_user_commands();
  while (1)
  { printf("\n---> ");
    argc = get_command_line(string_array);
    if (quit_command(string_array[0]))
    { exit_shell(string_array);
      break; }
    else if (!strcmp(string_array[0], "help"))
      print_user_commands();
    else if (!strcmp(string_array[0], "echo"))
      echo_commands(string_array, argc);
    else if (!strcmp(string_array[0], "run"))
      run_program(string_array, argc);
    else
      printf("command not found. "); } }

int main()
{ simple_shell(); }
