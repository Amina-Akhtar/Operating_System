#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<semaphore.h>

#define SIZE 20
#define SHM_KEY 1234

int main(int argc,char*argv[]) 
{
    int shmid = shmget(SHM_KEY, SIZE, 0666);
    if (shmid == -1) {
        perror("Error accessing shared memory");
        return 0;
    }
    char *buffer = (char *)shmat(shmid, NULL, 0);
    if (buffer == (char *)-1) {
        perror("Error attaching shared memory");
        return 0;
    }
    sem_t *sem_full = sem_open("full", 0);
    sem_t *sem_empty = sem_open("empty", 0);
    if (sem_full == SEM_FAILED || sem_empty == SEM_FAILED) {
        perror("Error opening semaphores");
        return 0;
    }
    while (1) 
    {
        sem_wait(sem_full); 
        if (buffer[0] == '$') 
        {  
            break;
        }
        printf("Consumer read: %s\n", buffer);
        printf("Press Enter to continue:");
        getchar();
        sem_post(sem_empty);  
    }
    shmdt(buffer);
    sem_close(sem_full);
    sem_close(sem_empty);
    printf("Consumer finished\n");
    return 0;
}

