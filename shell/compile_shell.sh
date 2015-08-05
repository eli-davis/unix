#!/bin/tcsh

echo "cc -c shell_command_line.c"
cc -c shell_command_line.c
if ($status != 0) exit

echo "cc -c shell_main.c"
cc -c shell_main.c
if ($status != 0) exit

echo "cc -g shell_command_line.o shell_main.o -o program"
cc -g shell_command_line.o shell_main.o -o shell
if ($status != 0) exit

echo "running shell \n\n"
./shell
