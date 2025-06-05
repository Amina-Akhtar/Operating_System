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

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        printf("File name is missing");
        return 0;
    }
    int fd=open(argv[1],O_RDONLY);
    if(fd<0)
    {
        perror("Error opening file");
        return 0;
    }
    int shmid = shmget(SHM_KEY, SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error creating shared memory");
        close(fd);
        return 0;
    }
    char *buffer = (char *)shmat(shmid, NULL, 0);
    if (buffer == (char *)-1) {
        perror("Error attaching shared memory");
        close(fd);
        return 0;
    }
    sem_t *sem_full = sem_open("full", O_CREAT, 0666, 0);
    sem_t *sem_empty = sem_open("empty", O_CREAT, 0666, 1);
    if (sem_full == SEM_FAILED || sem_empty == SEM_FAILED) {
        perror("Error creating semaphores");
        close(fd);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }
    char temp[SIZE + 1];
    size_t bytesRead;
    while ((bytesRead = read(fd, temp, SIZE)) > 0) 
    {
        temp[bytesRead] = '\0'; 
        sem_wait(sem_empty); 
        memset(buffer, 0, SIZE);
        strncpy(buffer, temp, SIZE);  
        printf("Producer write: %s\n", temp);
        sem_post(sem_full); 
    }
    sem_wait(sem_empty);
    buffer[0] = '$';
    sem_post(sem_full);
    close(fd);
    
    shmdt(buffer);
    shmctl(shmid, IPC_RMID, NULL);
    sem_close(sem_full);
    sem_close(sem_empty);
    sem_unlink("full");
    sem_unlink("empty");
    printf("Producer finished\n");
    return 0;
}

