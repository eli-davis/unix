#include "shell_library.h"
#include <stdio.h>
#include <string.h>


const int max_arg_c = 16;
const int max_arg_length = 16;
const int buffer_size = 256;

//private function. only implemented for \" currently
static char * handle_escape_char(char * start, char * current)
{ while (current != start)
  { *current = *(current-1);
    current -= 1; }
  return (start+1); }

//bool. only implemented for \" currently
static int escape_char(char * s)
{ if (s[0] == '\\' && s[1] == '\"')
    return 1;
  return 0; }

char * process_string(char * input)
{ int i;
  char * local_string, * temp, * result;
  static char * static_string = NULL;

  if (input != NULL)
    static_string = input;
  if (static_string == NULL)
    return NULL;

  while (static_string[0] == ' ')
    static_string += 1;
  local_string = static_string;

  if (local_string[0] == '\"')
  { static_string += 1;
    local_string += 1;
    while (local_string[0] != '\"' && local_string[0] != '\0')
    { if (escape_char(local_string))
      { static_string = handle_escape_char(static_string, local_string);
        local_string += 1; }
      local_string += 1;  } }
  else
  { while (local_string[0] != ' ' && local_string[0] != '\0')
    { if (escape_char(local_string))
        static_string = handle_escape_char(static_string, local_string);
      local_string += 1; } }

  result = static_string;

  if (local_string[0] == '\0')
    static_string = NULL;
  else
  { local_string[0] = '\0';
    static_string = local_string + 1; }

 return result;  }
 

int get_command_line(char * * array_of_strings)
{ int i, arg_c;
  char buffer[256];

  for (i = 0; i < buffer_size; i += 1)
  { buffer[i] = getchar();
    if (buffer[i] == '\n' || i == buffer_size-1)
    { buffer[i] = '\0';
      break; } }

  strcpy(array_of_strings[0], process_string(buffer));
  for (arg_c = 1; array_of_strings[arg_c-1] != NULL && arg_c < max_arg_c; arg_c += 1)
  { char * s = process_string(NULL);
    if (s != NULL)
      strcpy(array_of_strings[arg_c], s);
    else
      break; }
      
  return arg_c; }
