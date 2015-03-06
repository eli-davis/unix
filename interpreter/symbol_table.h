extern const int max_identifier_size;

void create_table();
void enlarge_table();
void print_table();

int hash(char * s);

int is_in_table(char * s, int hash_value, int count);
int  add_to_table(char * s, int n);

int  get_table_entry(char * s, int hash_value, int count);
void set_table_entry(char * s, int hash_value, int count, int n);
