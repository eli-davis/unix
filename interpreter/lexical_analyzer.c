#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexical_analyzer.h"

/* token types: */
const int identifier           = 101;
const int reserved_word        = 102;
const int string_constant      = 103;
const int integer_constant     = 104;
const int binary_operator      = 105;
const int comparison_operator  = 106;
const int assignment           = 107; // :=
const int open_parenthesis     = 108;
const int close_parenthesis    = 109;
const int open_curly_bracket   = 110;
const int close_curly_bracket  = 111;
const int semi_colon           = 112;
const int error                = 113;
const int end_of_file          = 114;

/* token identifiers: */
// reserved words
const int if_statement         = 201;
const int while_statement      = 202;
const int print_statement      = 203;
const int void_statement       = 204;
const int return_statement     = 205;
const int resultis_statement   = 206;
const int int_declaration      = 207;
const int string_declaration   = 208;
// binary operators
const int plus                 = 301;
const int minus                = 302;
const int multiply             = 303;
const int divide               = 304;
// comparison operators
const int less_than            = 401;
const int less_or_equal        = 402;
const int greater_than         = 403;
const int greater_or_equal     = 404;
const int equal                = 405;  // ==
const int not_equal            = 406;  // !=

typedef int bool;

FILE * f = 0;
position_in_file * current_line;

void allocate_position_in_file()
{ current_line = (position_in_file *)malloc(sizeof(position_in_file *));
  current_line->characters = (char *)malloc(512);
  current_line->line_number = (int)malloc(sizeof(int));
  current_line->character_number = (int)malloc(sizeof(int));
  current_line->length = (int)malloc(sizeof(int)); }

void set_position_in_file()
{ char * s = fgets(current_line->characters, 512, f);
  if (!s)
  { printf("file empty\n");
    exit(1); }
  current_line->length = strlen(current_line->characters);
  current_line->line_number = 0;
  current_line->character_number = 0; }

void open_file(char * file_name)
{ printf("%s\n\n", file_name);
  f = fopen(file_name, "r");
  if (!f)
  { printf("file name argument required\n");
    exit(1); }
  allocate_position_in_file();
  set_position_in_file(); }

void close_file()
{ fclose(f); }

bool is_string_character(char c)
{ if (c == '_' || (c >= 'a' && c <= 'z') || c >= 'A' && c <= 'Z')
    return 1;
  return 0; }

int is_reserved_word(char * s)
{ char * w = "while";
  char * i = "if";
  char * p = "print";
  char * v = "void";
  char * r = "return";
  char * x = "resultis";
  char * y = "int";
  char * z = "string";
  if (!strcmp(s, w))
    return while_statement;
  else if (!strcmp(s, i))
    return if_statement;
  else if (!strcmp(s, p))
    return print_statement;
  else if (!strcmp(s, v))
    return void_statement;
  else if (!strcmp(s, r))
    return return_statement;
  else if (!strcmp(s, x))
    return resultis_statement;
  else if (!strcmp(s, y))
    return int_declaration;
  else if (!strcmp(s, z))
    return string_declaration;
  else
    return 0; }

void error_message(char * message)
{ printf("line %d: ", current_line->line_number);
  printf("character %d ", current_line->character_number);
  printf("%s\n", message); }
  
token create_token(int token_type, int token_detail)
{ token t;
  t.type = token_type;
  t.detail.id = token_detail;
  return t; }

token create_string_token(char * s, int is_string_constant)
{ token t;
  if (is_string_constant)
  { t.type = string_constant;
    t.detail.name = s; }
  else if (is_reserved_word(s))
  { t.type = reserved_word;
    t.detail.id = is_reserved_word(s); }
  else
  { t.type = identifier;
    t.detail.name = s; }
  return t; }

token create_binary_operator_token(char c)
{ switch(c)
  { case '+':
      return create_token(binary_operator, plus);
    case '-':
      return create_token(binary_operator, minus);
    case '*':
      return create_token(binary_operator, multiply);
    case '/':
      return create_token(binary_operator, divide); }
  error_message("wrong argument passed to create_binary_operator_token\n");
  exit(1); }

char nextchar()
{ if (current_line->character_number == current_line->length)
  { char * s = fgets(current_line->characters, 512, f);
    if (!s)
    { printf("end of file\n");
      return 0; }
    current_line->length = strlen(current_line->characters);
    current_line->line_number += 1;
    current_line->character_number = 0; }
  int n = current_line->character_number;
  current_line->character_number += 1;
  return current_line->characters[n]; }

char backchar()
{ current_line->character_number -= 1; }

token get_token()
{ char c = nextchar();
  while (c == ' ' || c == '\n')
    c = nextchar();
  switch(c)
  { case 'a': case 'b': case 'c': case 'd': case 'e':
    case 'f': case 'g': case 'h': case 'i': case 'j':
    case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y':
    case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E':
    case 'F': case 'G': case 'H': case 'I': case 'J':
    case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y':
    case 'Z': case '_':
    { char temp_string[32];
      int i = 0;
      while (is_string_character(c))
      { temp_string[i] = c;
        i += 1;
        c = nextchar(); }
      backchar();
      temp_string[i] = 0;
      char * s = strdup(temp_string);
      return create_string_token(s, 0); }
      
    case '\"':
    { char temp_string[32];
      c = nextchar();
      int pos = 0;
      while (pos < 31 && c != '\"')
      { temp_string[pos] = c;
        pos += 1;
        c = nextchar(); }
      temp_string[pos] = 0;
      if (pos == 31)
      { error_message("max string length is 31 characters\n");
        exit(1); }
      char * s = strdup(temp_string);
      return create_string_token(s, 1); }

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    { int value = c - '0';
      c = nextchar();
      while (c >= '0' && c <= '9')
      { value = value * 10 + c - '0';
        c = nextchar(); }
      backchar();
      return create_token(integer_constant, value); }

    case '+': case '-': case '*':
     return create_binary_operator_token(c);

    case '/':
    { c = nextchar();
      if (c == ' ')
        return create_binary_operator_token(c);
      if (c == '/')
      { current_line->character_number = current_line->length;
        return get_token(); }
      else
      { error_message("unrecognized use of '/' character\n");
        exit(1); } }

    case '=':
    { c = nextchar();
      if (c == ' ')
      { error_message("for initialization and assignment use :=");
        exit(1); }
      else if (c == '=')
        return create_token(comparison_operator, equal); }

    case '!':
    { c = nextchar();
      if (c == '=')
        return create_token(comparison_operator, not_equal);
      else
      { error_message("undefined use of '!' character");
        exit(1); } }

    case '<':
    { c = nextchar();
      if (c == '=')
        return create_token(comparison_operator, less_or_equal);
      backchar();
      return create_token(comparison_operator, less_than); }

    case '>':
    { c = nextchar();
      if (c == '=')
        return create_token(comparison_operator, greater_or_equal);
      backchar();
      return create_token(comparison_operator, greater_than); }

    case ':':
    { c = nextchar();
      if (c != '=')
      { error_message("did you mean :=?");
        exit(1); }
      else
        return create_token(assignment, 0); }

    case '(':
      return create_token(open_parenthesis, 0);

    case ')':
      return create_token(close_parenthesis, 0);

    case '{':
      return create_token(open_curly_bracket, 0);

    case '}':
      return create_token(close_curly_bracket, 0);

    case ';':
      return create_token(semi_colon, 0);

    case(0):
      return create_token(end_of_file, 0);

    //todo: return error token
    default:
    { error_message("");
      printf("character %c unrecognized\n", c);
      exit(1); }
  }
}




    
