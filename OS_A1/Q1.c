#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define DELIMITER " \t\r\n"

void executeCommand(char *command) 
{
    char *args[100];
    int i=0;
    char *token = strtok(command, DELIMITER);
    while (token != NULL) 
    {
        args[i++] = token;
        token = strtok(NULL, DELIMITER);
    }
    args[i] = NULL; 
    if (args[0] == NULL) 
    {
      return; 
    }
    if (strcmp(args[0], "cd") == 0) 
    {
        if (i < 2) 
        {
            fprintf(stderr, "cd: missing argument\n");
        } 
        else if (chdir(args[1]) != 0) 
        {
            perror("cd");
        }
        return;
    }
    pid_t process = fork();
    if (process == -1) {
        perror("Fork failed!");
        exit(EXIT_FAILURE);
    }

    if (process == 0)// Child process
    {
        execlp(args[0], args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } 
    else if(process>0)// Parent process
    {
        int status;
        waitpid(process, &status, 0);
    }
}

int main() 
{
    char input[MAX_INPUT];

    while (1) 
    {
        printf("myshell> ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) 
        {
            perror("fgets");
            exit(EXIT_FAILURE);
        }
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "exit") == 0) 
        {
            break;
        }
        executeCommand(input);
    }
    printf("Exited myshell!\n");
    return 0;
}
