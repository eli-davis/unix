#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "backup_working_directory.h"

bool is_a_file(char * name)
{ struct stat S;
  int r;
  r = stat(name, & S);
  if (r < 0)
    return 0;
  if (S_ISREG(S.st_mode))
    return 1;
  return 0; }

bool is_a_backup_directory(struct dirent * directory_entry)
{ int i;
  char * backup = "backup-";
  if ( directory_entry->d_namlen == backup_directory_name_length && !is_a_file(directory_entry->d_name))
  { for (i = 0; i < 7; i += 1)
    { if (directory_entry->d_name[i] != backup[i])
        return 0; }
    return 1; }
  return 0; }

bool has_been_modified(char * name, int most_recent_backup_time)
{ int time_last_modified = get_time_last_modified(name);
  if (time_last_modified > most_recent_backup_time)
    return 1;
  return 0; }

int get_time_created(struct dirent * directory_entry)
{ struct stat S;
  int r;
  r = stat(directory_entry->d_name, & S);
  if (r < 0)
  { perror("stat");
    exit(1); }
  //printf("time created = %d\n", S.st_birthtimespec.tv_sec);
  return S.st_birthtimespec.tv_sec; }

int get_time_last_modified(char * name)
{ struct stat S;
  int r;
  r = stat(name, & S);
  if (r < 0)
    return 0;
  //printf("time last modified = %d\n", S.st_mtimespec.tv_sec);
  return S.st_mtimespec.tv_sec; }

int get_time_most_recent_backup(DIR * current_directory)
{ struct dirent * directory_entry = readdir(current_directory);
  int most_recent_backup_time = 0;
  while(directory_entry)
  { //print_directory_entry(directory_entry);
    if (is_a_backup_directory(directory_entry))
    { int backup_time = get_time_created(directory_entry);
      if (backup_time > most_recent_backup_time)
        most_recent_backup_time = backup_time; }
    directory_entry = readdir(current_directory); }
  //printf("\n\nmost recent backup time %d\n", most_recent_backup_time);
  return most_recent_backup_time; }

char * get_current_time()
{ time_t current_time = time(NULL);
  char * time_string = ctime(&current_time);
  if (!time_string)
    perror("time string not created by ctime");
  char * new_time_string = strdup(time_string);
  return new_time_string; }

char * create_backup_directory_name()
{ char * directory_name = (char *)malloc(sizeof(char) * 33);
  char * time_string = get_current_time();
  int i;
  for (i=0; i<25; i+=1)
    if (time_string[i] == '\n')
      time_string[i] = 0;
    else if (time_string[i] == ' ')
    { time_string[i] = '_';
      //chage extra space to leading '0' in single digit dates
      //Nov__8 would become Nov_08
      if (time_string[i-1] == '_')
        time_string[i] = '0'; }
  strncpy(directory_name, "backup-", 7);
  strncat(directory_name, time_string, 24);
  free(time_string);
  return directory_name; }

char * create_backup_directory()
{ char * directory_name = create_backup_directory_name();
  int make_directory = mkdir(directory_name, 0755); //permissions: 111 101 101
  if (make_directory < 0)
    perror("failed to create backup directory");
  return directory_name; }

char * create_backup_file_path(char * backup_directory_name, char * file_name)
{ char * backup_file_path = (char *)malloc(sizeof(char) * 64);
  strncpy(backup_file_path, backup_directory_name , 31);
  strncat(backup_file_path, "/", 1);
  strncat(backup_file_path, file_name, 31);
  return backup_file_path; }

DIR * open_current_working_directory()
{ char s[80];
  char * current_working_directory_name = getcwd(s, 80);
  //printf("directory name %s\n", current_working_directory_name);
  if (!current_working_directory_name)
    perror("getcwd() failed. did you call rm -rf on a directory? "
           "if so 'cd ..' and reenter current working directory.\n");
  DIR * current_working_directory = opendir(current_working_directory_name);
  return current_working_directory; }

void copy_file_to_backup_directory(char * backup_file_path, char * file_name)
{ int i, r, w;
  int reading_file = open(file_name, O_RDONLY);
  if (reading_file < 0)
    perror("failed to open file for reading\n");
  int writing_file = open(backup_file_path, O_WRONLY | O_CREAT, 0755);
  if (writing_file < 0)
    perror("failed to open file for writing\n");
  char buffer[512];
  while (1)
  { r = read(reading_file, buffer, sizeof(buffer));
    if (r <= 0)
      break;
    w = write(writing_file, buffer, r);
    if (w != r)
    { printf("failed writing to file %s\n", backup_file_path);
      break;  } }
  close(reading_file);
  close(writing_file); }

void backup_modified_files(int most_recent_backup_time, char * argv[])
{ char * backup_directory_name = NULL;
  int i;
  for (i = 1; argv[i]; i += 1)
  { char * file_name = argv[i];
    if (is_a_file(file_name) && has_been_modified(file_name, most_recent_backup_time))
    { if (!backup_directory_name)
        backup_directory_name = create_backup_directory();
      char * backup_file_path = create_backup_file_path(backup_directory_name, file_name);
      copy_file_to_backup_directory(backup_file_path, file_name); } } }

void backup_directory(int most_recent_backup_time)
{ char * backup_directory_name = NULL;
  DIR * current_working_directory = open_current_working_directory();
  struct dirent * directory_entry = readdir(current_working_directory);
  while(directory_entry)
  { char * file_name = directory_entry->d_name;
    if (is_a_file(directory_entry->d_name) && has_been_modified(file_name, most_recent_backup_time))
    { if (!backup_directory_name)
        backup_directory_name = create_backup_directory();
      char * backup_file_path = create_backup_file_path(backup_directory_name, file_name);
      copy_file_to_backup_directory(backup_file_path, file_name); }
    directory_entry = readdir(current_working_directory); }
  closedir(current_working_directory); }

//debugging
void print_directory_entry(struct dirent * directory_entry)
{ printf("\n\n");
  printf("file number %d\n", directory_entry->d_fileno);
  printf("length      %hd\n", directory_entry->d_reclen);
  printf("type        %hhd\n", directory_entry->d_type);
  printf("name length %hhd\n", directory_entry->d_namlen);
  printf("name        %s\n", directory_entry->d_name); }

int main(int argc, char * argv[])
{ DIR * current_working_directory = open_current_working_directory();
  int most_recent_backup_time = get_time_most_recent_backup(current_working_directory);
  closedir(current_working_directory);
  if (argc > 1)
    backup_modified_files(most_recent_backup_time, argv);
  else
    backup_directory(most_recent_backup_time); }
