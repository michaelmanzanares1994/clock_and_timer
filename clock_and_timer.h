#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT 10
#define BUFFER_SIZE 12
#define PIPE_SIZE 2

void print_clock();
void print_timer(int start);
void create_pipes();
void create_processes(int time);
void close_processes();
void close_program();
