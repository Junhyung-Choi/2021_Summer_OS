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
void* start_write(void* arg);
void* end_write(void* arg);

int main(void)
{
    monitorInit();
    pthread_t t1,t2,t3,t4;

    printf("Monitor Start! \n");

    pthread_create(&t1,NULL,start_write,NULL);
    //sleep(1);
    pthread_create(&t2,NULL,start_write,NULL);
    sleep(1);
    pthread_create(&t3,NULL,end_write,NULL);
    //sleep(1);
    pthread_create(&t4,NULL,end_write,NULL);
    //sleep(1);
    
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
    printf("write_wait(): called\n");
    monitor.write_count++;
    printf("write_wait(): write_count++ called\n");
    if (monitor.next_count > 0)
    {
        sem_post(&monitor.next);
        printf("write_wait(): next semaphore lock solved\n");
    }
    else
    {
        sem_post(&monitor.mutex);
        printf("write_wait(): mutex semaphore lock solved\n");
    }
    printf("write_wait(): write semaphore lock \n");
    printf("write_wait(): WAITING WRITING SIGNAL.........................................\n");
    sem_wait(&monitor.write);
    printf("write_wait(): write semaphore lock passed\n");
    monitor.write_count--;
    printf("write_wait(): write_count-- called\n");

}

void write_post()
{   
    printf("write_post(): write_post() called\n");
    if (monitor.write_count > 0)
    {
        printf("write_post(): if passed\n");
        monitor.next_count++;
        printf("write_post(): next_count++ called\n");
        printf("write_post(): POSTING WRITING SIGNAL.............................................\n");
        sem_post(&monitor.write);
        printf("write_post(): write semaphore lock solved\n");
        printf("write_post(): next semaphore lock start \n");
        sem_wait(&monitor.next);
        printf("write_post(): next semaphore lock passed \n");
        monitor.next_count--;
        printf("write_post(): next_count-- called \n");
    }
    printf("write_post(): write_post() end\n");
}

void* start_write(void* arg)
{
    printf("write thread: start\n");
    printf("write thread: mutex lock start\n");
    sem_wait(&monitor.mutex);
    printf("write thread: mutex lock passed\n");
    write_wait();
    printf("write_wait(): WRITING POST RECEIVED / WRITING SOMETHING..........................\n");
    if (monitor.next_count > 0)
    {
        sem_post(&monitor.next);
        printf("write thread: next lock solved\n");
    }
    else
    {
        sem_post(&monitor.mutex);
        printf("write thread: mutex lock solved\n");
    }
    printf("write thread end\n");
    return NULL;
}

void* end_write(void* arg)
{
    printf("end thread start\n");
    printf("end thread: mutex lock start\n");
    sem_wait(&monitor.mutex);
    printf("end thread: mutex lock passed\n");
    write_post();
    printf("write_post(): END WRITING......................................\n");
    if (monitor.next_count > 0)
    {
        sem_post(&monitor.next);
        printf("end thread: next lock solved\n");
    }
    else
    {
        sem_post(&monitor.mutex);
        printf("end thread: mutex lock solved\n");
    }
    printf("end thread end\n");
    return NULL;
}
