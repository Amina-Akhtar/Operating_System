#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

typedef struct {
    int id;
    int type;
} Customer;

sem_t vipSeats;
sem_t regSeats;
pthread_mutex_t mutex;
int seatNum=1;
void* bookSeat(void* arg) 
{
    Customer* cust = (Customer*)arg;
    pthread_mutex_lock(&mutex);
    if (cust->type) 
    {
        if (sem_trywait(&vipSeats) == 0) 
        {
            printf("VIP customer %d booked VIP seat %d\n", cust->id,seatNum++);
        } 
        else if (sem_trywait(&regSeats) == 0) 
        {
            printf("VIP customer %d booked regular seat %d\n", cust->id,seatNum++);
        } 
        else {
            printf("VIP customer %d no seat available\n", cust->id);
        }
    } 
    else 
    {
        if (sem_trywait(&regSeats) == 0) 
        {
            printf("Regular customer %d  booked regular seat %d\n", cust->id,seatNum++);
        } 
        else {
            printf("Regular customer %d no seats available\n", cust->id);
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    int vip_seat, reg_seat, total_cust;
    printf("Enter the number of VIP seats:");
    scanf("%d", &vip_seat);
    printf("Enter the number of regular seats:");
    scanf("%d", &reg_seat);
    printf("Enter the total number of customers:");
    scanf("%d", &total_cust);   
    sem_init(&vipSeats, 0, vip_seat);
    sem_init(&regSeats, 0, reg_seat);   
    pthread_mutex_init(&mutex, NULL);
    pthread_t customers[total_cust];
    Customer data[total_cust];
    for (int i = 0; i < total_cust; i++) 
    {
        printf("Enter data for Customer %d (1-VIP 0-Regular): ", i + 1);
        data[i].id = i + 1;
        scanf("%d", &data[i].type);
    }
    printf("\n");
    for (int i = 0; i < total_cust; i++) 
    {
        pthread_create(&customers[i], NULL, bookSeat, &data[i]);
    }
    for (int i = 0; i < total_cust; i++) 
    {
        pthread_join(customers[i], NULL);
    }
    sem_destroy(&vipSeats);
    sem_destroy(&regSeats);
    pthread_mutex_destroy(&mutex);
    return 0;
}


