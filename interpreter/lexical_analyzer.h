/* token types: */
extern const int identifier;
extern const int reserved_word;
extern const int string_constant;
extern const int integer_constant;
extern const int binary_operator;
extern const int comparison_operator;
extern const int assignment;           // :=
extern const int open_parenthesis;
extern const int close_parenthesis;
extern const int open_curly_bracket;
extern const int close_curly_bracket;
extern const int semi_colon;
extern const int error;
extern const int end_of_file;

/* token identifiers: */
// reserved words
extern const int if_statement;
extern const int while_statement;
extern const int print_statement;
extern const int void_statement;
extern const int return_statement;
extern const int resultis_statement;
extern const int int_declaration;
extern const int string_declaration;
// binary operators
extern const int plus;
extern const int minus;
extern const int multiply;
extern const int divide;
// comparison operators
extern const int less_than;
extern const int less_or_equal;
extern const int greater_than;
extern const int greater_or_equal;
extern const int equal;               // ==
extern const int not_equal;

typedef struct _position_in_file
{ char * characters; // a line from the file
  int line_number;
  int character_number;
  int length; } position_in_file;

typedef struct _token
{ int type;
  union { int id; char * name; } detail;
} token;

token get_token();
void open_file(char *);
void close_file();
void print_type(token);
