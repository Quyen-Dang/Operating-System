#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

sem_t wrt;
sem_t lock;
int cnt = 1;
int numreader = 0;

void init()
{
    sem_init(&lock, 0, 1);
    sem_init(&wrt, 0, 1);
}

void deinit()
{
    sem_destroy(&lock);
    sem_destroy(&wrt);
}

void *writer(void *wno)
{
    sem_wait(&wrt);
    cnt = cnt + 1;
    printf("Writer %d modified cnt to %d\n", (*((int *)wno)), cnt);
    sem_post(&wrt);
}
void *reader(void *rno)
{
    // Reader acquire the lock before modifying numreader
    sem_wait(&lock);
    numreader++;
    if (numreader == 1)
    {
        sem_wait(&wrt); // If this id the first reader, then it will block the writer
    }
    sem_post(&lock);
    // Reading Section
    printf("Reader %d: read cnt as %d\n", *((int *)rno), cnt);

    // Reader acquire the lock before modifying numreader
    sem_wait(&lock);
    numreader--;
    if (numreader == 0)
    {
        sem_post(&wrt); // If this is the last reader, it will wake up the writer.
    }
    sem_post(&lock);
}