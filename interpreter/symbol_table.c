#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

const int max_identifier_size = 32;

typedef struct _symbol
{ char * name;
  int value; } symbol;

int table_size = 16;
symbol * * table;

void create_table()
{ int i;
  table = (symbol * *)malloc(table_size * sizeof(symbol *));
  for (i = 0; i < table_size; i += 1)
  { table[i] = (symbol *)malloc(sizeof(symbol));
    table[i]->name = 0;
    table[i]->value = 0; } }

void enlarge_table()
{ int i, old_table_size = table_size;
  symbol * * old_table = table;
  table_size *= 2;
  create_table();
  for (i = 0; i < old_table_size; i += 1)
    if (old_table[i]->name)
      add_to_table(old_table[i]->name, old_table[i]->value);
  for (i = 0; i < old_table_size; i += 1)
    free(old_table[i]);
  free(old_table); }

void print_table()
{ int i;
  for (i = 0; i < table_size; i += 1)
    printf("%d: %s\n", i, table[i]->name); }

//todo: return error token
int hash(char * s)
{ int i, length = strlen(s);
  if (length >= max_identifier_size - 1)
  { printf("error: string is too long\n");
    exit(1); }
  int value = 0;
  for (i = 0; i < length; i += 1)
  { value <<= 1;
    value ^= s[i];
    if (value < 0) value = -value;
    value %= table_size; }
  return value; }

//todo: return error token
int add_to_table(char * s, int v)
{ int h = hash(s);
  if (!is_in_table(s, h, 0))
  { set_table_entry(s, v, h, 0);
    return 1; }
  printf("error: string exists in table\n");
  return 0; }

int is_in_table(char * s, int hash_value, int count)
{ if (hash_value == table_size)
    hash_value = 0;
  if (table[hash_value]->name == 0 || count == table_size)
    return 0;
  if (!strcmp(s, table[hash_value]->name))
    return 1;
  return is_in_table(s, hash_value + 1, count + 1); }

//todo: return error token
int get_table_entry(char * s, int hash_value, int count)
{ if (hash_value == table_size)
    hash_value = 0;
  if (table[hash_value]->name == 0 || count == table_size)
  { printf("error: identifier not found\n");
    exit(1); }
  if (!strcmp(s, table[hash_value]->name))
    return table[hash_value]->value;
  else
    return get_table_entry(s, hash_value + 1, count + 1); }

void set_table_entry(char * s, int v, int hash_value, int count)
{ if (count == table_size >> 1)
  { enlarge_table();
    int h = hash(s);
    set_table_entry(s, v, h, 0);
    return; }
  if (hash_value == table_size)
    hash_value = 0;
  if (table[hash_value]->name == 0)
  { int sz = strlen(s);
    table[hash_value]->name = (char *)malloc(sz + 1);
    strncpy(table[hash_value]->name, s, max_identifier_size-1);
    table[hash_value]->value = v; }
  else
    set_table_entry(s, v, hash_value + 1, count + 1); }
