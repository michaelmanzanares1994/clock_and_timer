
/*
 Michael Manzanares
 Stephen Summy
 CS570 Operating Systems
 Dr. Leonard
 Assigment 3

 Synopsis- For this assignment, I had to implement multiple processes and time viewer.
 I had to implement three process, first process acts as a clock and prints the
 time every second. The second child process runs a countdown timer that prints
 out every minutes and seconds once every second until it reaches 00:00. And all of these
 Processes are communicating via pipes accordingly. Once the third process finishes,(reaches 00:00)
 it send a signal via pipes to kill its sibling processes and then terminates itself.
 */

/*Global Variables*/
int first_pipe[PIPE_SIZE];
int second_pipe[PIPE_SIZE];
char buffer[BUFFER_SIZE];

/*print_clock Function*/
void print_clock(){
    char string[BUFFER_SIZE];
    strcpy(string, "time to die");

    time_t now;
    struct tm *current;

    printf("Child 1 is Running...\n");
    /*Close unused pipes with error checking*/
    if (close(first_pipe[0]) == -1){
        printf("Error closing reading end of pipe 1\n");
        _exit(1);
    }
	if (close(second_pipe[1]) == -1){
        printf("Error closing writing end of pipe 2\n");
        _exit(1);
    }
	if (close(first_pipe[1]) == -1){
        printf("Error closing writing end of pipe 1\n");
        _exit(1);
    }
    /*For loop to display time every second*/
    for(;;){
        now = time(NULL);
        current = localtime(&now);

       long status = read(second_pipe[0], buffer, 12);
        //compare buffer
        int ret = strcmp(string, buffer);
        if (ret != 0) {
            printf("The time is %d:%d:%d\n", current->tm_hour, current->tm_min, current->tm_sec);
            sleep(1);
        }
        /*If quit message came through*/
	if(ret == 0){
            //close writing end
            if (close(second_pipe[0]) == -1){
                printf("Error closing writing end of pipe 2\n");
                _exit(1);
            }

            //return back to parent
            printf("End of Clock(child 1) process...\n");
            exit(1);
        }
    }
}
void print_timer(int start){
    /*string signal to end process*/
    strcpy(buffer, "time to die");
    printf("Child 2 is Running...\n");
    //close unused pipes
    if (close(first_pipe[0]) == -1){
        printf("Error closing reading end of pipe 3\n");
        _exit(1);
    }
	if (close(first_pipe[1]) == -1){
        printf("Error closing writing end of pipe 1\n");
        _exit(1);
    }

    if (close(second_pipe[0]) == -1){
        printf("Error closing reading end of pipe 1\n");
        _exit(1);
    }
    //count down
    for( ;start>0; start--){
		printf("Count Down: 00:%d\n", start);
        //send signal to child 1 and child 2 process not to end
        write(second_pipe[1], "Not dead yet",12);
		sleep(1);
    }
    //signal to siblings to DIE
    write(second_pipe[1], buffer, sizeof(buffer));

    //close pipe 3 writing end
    if (close(second_pipe[1]) == -1){
        printf("Error closing writing end of pipe 2");
        _exit(EXIT_FAILURE);
    }
    printf("Count Down: 00:00\n");
    exit(1);
}
void create_pipes(){
/*Create pipe 1 with error checking*/
    if (pipe(first_pipe) == -1){
        printf("Pipe 1 failed to open!\n");
        exit(1);
    }
    /*Create pipe 2 with error checking*/
    if (pipe(second_pipe) == -1){
        printf("Pipe 2 failed to open!\n");
        exit(1);
    }
}
void create_processes(int time){

  /*Fork child 1 with error check*/
   switch (fork()){
       case -1:
           printf("Error forking child 1!\n");
           exit(1);
       case 0:
           print_clock();
           exit(1);
       default:
           break;
   }
  /*Fork child 2 with error check*/
  switch (fork()) {
      case -1:
          printf("Error forking child 2!\n");
          exit(1);
          break;
      case 0:
          print_timer(time);
          exit(1);
      default:
          break;
  }
}

void close_processes(){
    /*Close pipes with error checking*/
    if (close(first_pipe[0]) == -1){
        printf("Error closing reading end of the pipe 1\n");
        exit(EXIT_FAILURE);
    }
    if (close(first_pipe[1]) == -1){
        printf("Error closing writing end of the pipe 1\n");
        exit(EXIT_FAILURE);
    }
    if (close(second_pipe[0]) == -1){
        printf("Error closing reading end of the pipe 2\n");
        exit(EXIT_FAILURE);
    }
    if (close(second_pipe[1]) == -1){
        printf("Error closing writing end of the pipe 2\n");
        exit(EXIT_FAILURE);
    }

}
void close_program(){
    /*Wait for child processes to terminate*/
    if (wait(NULL) == -1){
        printf("Error waiting, no 1 child process is ended!\n");
        exit(EXIT_FAILURE);
    }
    if (wait(NULL) == -1){
        printf("Error waiting, no 2 child process is ended!\n");
        exit(EXIT_FAILURE);
    }

    printf("Time to print out a Friendly Message!\n");
    printf("Parent process finishing\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, const char * argv[]){
    int time = DEFAULT; //set the default time to 10 if no args passed.

    if (argc == 2) time = atoi(argv[1]);

    create_pipes();
    printf("Pipes opened, now creating child process!\n");

    create_processes(time);
    printf("Parent closing pipes\n");

    close_processes();
    printf("Parent waiting for children completion...\n");

    close_program();
}
