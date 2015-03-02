#include<dirent.h>

typedef unsigned char byte;
typedef int bool;

byte backup_directory_name_length = 31; //compared with: u_int8 dirent.d_namlen

bool is_a_file(char * name);
bool is_a_backup_directory(struct dirent * directory_entry);
bool has_been_modified(char * name, int most_recent_backup_time);

int get_time_created(struct dirent * directory_entry);
int get_time_last_modified(char * name);
int get_time_most_recent_backup(DIR * current_directory);

char * get_current_time();
char * create_backup_directory_name();
char * create_backup_directory(); //returns name of directory
char * create_backup_file_path(char * backup_directory_name, char * file_name); 

DIR * open_current_working_directory();

void copy_file_to_backup_directory(char * backup_file_path, char * file_name);  
void backup_modified_files(int most_recent_backup_time, char * argv[]);
void backup_directory();
void print_directory_entry(struct dirent * directory_entry);
