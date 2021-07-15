// Junhyung_Choi, 2021-07-14 
// 2021 Summer Operation Software(202115HY20024)
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


// Define Monitor sturct contains semaphore
typedef struct Monitor_Type
{
    sem_t mutex;
    sem_t next;
    sem_t write;
    int next_count;
    int write_count;
}Monitor_Type;

// Declare monitor struct as a global variable so that functions can share it.
Monitor_Type monitor;

int monitorInit();
void monitorDestroy();
void write_wait();
void write_post();
void* write_ready(void* arg);
void* write_signal(void* arg);

int main(void)
{
    monitorInit();
    pthread_t t1,t2,t3,t4;

    printf("Monitor Start! \n");

    pthread_create(&t1,NULL,write_ready,NULL);
    pthread_create(&t2,NULL,write_ready,NULL);
    sleep(1); // call sleep() to makes sure sem_wait() and sem_post() works in a set order  
    pthread_create(&t3,NULL,write_signal,NULL);
    pthread_create(&t4,NULL,write_signal,NULL);
    
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);

    printf("Monitor End! \n");
    
    monitorDestroy();

    return 0;
}

int monitorInit()
{
    int ret = 1;
    monitor.next_count = 0;
    monitor.write_count = 0;
    // initialize mutex semaphore to 1 so that first procedure can pass wait
    // next, write semaphore must need sem_post() to pass wait
    if ((sem_init(&monitor.mutex,0,1) == 0) && (sem_init(&monitor.next,0,0) == 0) && (sem_init(&monitor.write,0,0) == 0))
        ret = 0;
    else
        printf("ERROR: Unable to initialize semaphores\n");
    return ret;
}

void monitorDestroy()
{
    sem_destroy(&monitor.mutex);
    sem_destroy(&monitor.next);
    sem_destroy(&monitor.write);
}

void write_wait()
{
    monitor.write_count++;
    if (monitor.next_count > 0)
        sem_post(&monitor.next);
    else
        sem_post(&monitor.mutex);
    printf("write_wait(): WAITING WRITING POST SIGNAL...............\n");
    sem_wait(&monitor.write);
    printf("write_wait(): RECEIVED WRITING POST SIGNAL...............\n");
    monitor.write_count--;

}

void write_post()
{   
    if (monitor.write_count > 0)
    {
        monitor.next_count++;
        printf("write_post(): SENDING WRITING POST SIGNAL.................\n");
        sem_post(&monitor.write);
        sem_wait(&monitor.next);
        monitor.next_count--;
    }
}

void* write_ready(void* arg)
{
    sem_wait(&monitor.mutex);
    write_wait();
    printf("write_ready(): WRITING SOMETHING...............\n");
    printf("write_ready(): END WRITING...............\n");
    if (monitor.next_count > 0)
        sem_post(&monitor.next);
    else
        sem_post(&monitor.mutex);
    return NULL;
}

void* write_signal(void* arg)
{
    sem_wait(&monitor.mutex);
    write_post();
    if (monitor.next_count > 0)
        sem_post(&monitor.next);
    else
        sem_post(&monitor.mutex);
    return NULL;
}
