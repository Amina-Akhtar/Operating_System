#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_ATTEMPTS 3
#define FAILURE_CHANCE 30

int simulate_failure() {
    return rand() % 100 < FAILURE_CHANCE;
}

int execute_stage(const char *stage_name, int sleep_time) {
    printf("Starting stage: %s\n", stage_name);
    sleep(sleep_time);  
    if (simulate_failure()) {
        printf("Stage %s failed!\n", stage_name);
        return 1; 
    }
   
    printf("Stage %s completed successfully!\n", stage_name);
    return 0;  
}

int main() {
    srand(time(NULL)); 
   
    const char *stages[] = {"Material Preparation", "Assembly", "Quality Check", "Packaging"};
    int stage_times[] = {2, 1, 2, 3}; 
    int num_stages = 4;
    int attempts[num_stages];  
    int status;
   
    for (int i = 0; i < num_stages; i++) {
        attempts[i] = 0;
    }
    for (int i = 0; i < num_stages; i++) {
        int success = 0;
        int attempt = 0;
       
        while (attempt < MAX_ATTEMPTS && !success) {
            attempt++;
            attempts[i] = attempt;
           
            pid_t pid = fork();
           
            if (pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                int result = execute_stage(stages[i], stage_times[i]);
                exit(result);  
            } else {
                waitpid(pid, &status, 0);
               
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) 
                {
                    success = 1;
                    printf("%s completed after %d attempt(s)\n", stages[i], attempt);
                } else {
                    printf("%s failed on attempt %d\n", stages[i], attempt);
                }
            }
        }
       
        if (!success) {
            printf("Stage %s failed after %d attempts. Terminating production line.\n", stages[i], MAX_ATTEMPTS);
            exit(EXIT_FAILURE);
        }
    }
    printf("\nProduction line completed successfully!\n");
    printf("Summary Report:\n");
    for (int i = 0; i < num_stages; i++)
    {
        printf("Stage: %s, Attempts: %d\n", stages[i], attempts[i]);
    }
    return 0;
}


