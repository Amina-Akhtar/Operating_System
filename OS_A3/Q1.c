#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<math.h>
#include<string.h>
#include<fcntl.h> 

#define GUARDIAN -9999999
#define MAX_DATA 10000

int rawData[MAX_DATA]; 
int data[MAX_DATA]; 
int rawCount = 0; 
int count = 0; 
double mean = 0.0, median = 0.0, stdDev = 0.0; 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dataLoaded = PTHREAD_COND_INITIALIZER;
pthread_cond_t dataCleaned = PTHREAD_COND_INITIALIZER;

void* loadData(void* arg) 
{ 
    int fd = open("input.txt", O_RDONLY);
    if (fd < 0) 
    {
        perror("Error opening file");
        return NULL;
    }
    pthread_mutex_lock(&mutex);
    char buffer[1024];
    int bytesRead, i = 0;
    char temp[32];
    int index = 0;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n' || buffer[i] == ' ') 
            {
                if (index > 0) 
                {
                    temp[index] = '\0';
                    rawData[rawCount++] = atoi(temp);
                    index = 0; 
                }
            } 
            else 
            {
                temp[index++] = buffer[i];
            }
        }
    }
    if (index > 0) 
    {
        temp[index] = '\0';
        rawData[rawCount++] = atoi(temp);
    }
    close(fd);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&dataLoaded);
    return NULL;
}

void* cleanData(void* arg) {
     pthread_mutex_lock(&mutex);
     while (rawCount == 0) {
        pthread_cond_wait(&dataLoaded, &mutex);
    }
     for (int i = 0; i < rawCount; i++) {
        if (rawData[i] != GUARDIAN) {
            data[count++] = rawData[i];
        }
    }
    for (int i = 0; i < count ; i++) {
        for (int j = 0; j < count; j++) {
            if (data[i] < data[j]) {
                int temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
    int unique = 0;
    int temp[MAX_DATA];
    temp[unique++] = data[0];
    for (int i = 1; i < count; i++) {
        if (data[i] != data[i - 1]) {
            temp[unique++] = data[i];
        }
    }
    count = unique;
    memcpy(data, temp, sizeof(int) * count);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&dataCleaned);
    return NULL;
}

void* analyzeData(void* arg) {
    pthread_mutex_lock(&mutex);
    while (count == 0) {
        pthread_cond_wait(&dataCleaned, &mutex);
    }
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += data[i];
    }
    mean = sum / count;
    if (count % 2 == 0) {
        median = (data[count / 2 - 1] + data[count / 2]) / 2.0;
    } 
    else {
        median = data[count / 2];
    }
    double variance = 0.0;
    for (int i = 0; i < count; i++) {
        variance += (data[i] - mean) * (data[i] - mean);
    }
    variance /= count;
    stdDev = sqrt(variance);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* reportData(void* arg) {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Error opening file");
        return NULL;
    }
    pthread_mutex_lock(&mutex);
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "{'Cleaned Data': [");
    write(fd, buffer, strlen(buffer));
    for (int i = 0; i < count; i++) {
       if(i==count-1)
       {
        snprintf(buffer, sizeof(buffer), "%d],\n", data[i]);
        write(fd, buffer, strlen(buffer));
       }
       else
       {
        snprintf(buffer, sizeof(buffer), "%d,\n", data[i]);
        write(fd, buffer, strlen(buffer));
      }
    }
    snprintf(buffer, sizeof(buffer), "'Mean' : %.2f,\n'Median' : %.2f,\n'Standard Deviation' : %.2f}\n", mean, median, stdDev);
    write(fd, buffer, strlen(buffer));
    close(fd);
    return NULL;
}

int main(int argc, char* argv[]) 
{
   //use -lm to compile
    pthread_t threads[4];
    pthread_create(&threads[0], NULL, loadData, NULL);
    pthread_create(&threads[1], NULL, cleanData, NULL);
    pthread_create(&threads[2], NULL, analyzeData, NULL);
    pthread_create(&threads[3], NULL, reportData, NULL);
    for (int i = 0; i < 4; i++) 
    {
        pthread_join(threads[i], NULL);
    }
    printf("Parallel data exploration quest completed\n");
    return 0;
}

