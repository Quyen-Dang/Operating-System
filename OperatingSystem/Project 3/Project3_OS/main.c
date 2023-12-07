#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <semaphore.h>
#include "readerwriter.c"
//idea citation:
//http://shivammitra.com/reader-writer-problem-in-c/#
//https://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf
//https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/rwlock.c

int main()
{
    init();

    pthread_t rlock[10], wlock[10];

    FILE *fp = fopen("scenarios.txt", "r");
    if (fp == NULL)
    {
        printf("File open error!");
        exit(EXIT_FAILURE);
    }
    int rcount = 0;
    int wcount = 0;

    char *scenario = NULL;
    size_t len = 0;
    ssize_t read;

    int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; //Just used for numbering the producer and consumer

    while ((read = getline(&scenario, &len, fp)) != -1)
    {
        printf("Simulation start...\n\n");
        printf("Scenario: %s\n", scenario);
        
        for (int i = 0; i < read; i++)
        {
            if (scenario[i] == 'r')
            {
                pthread_create(&rlock[rcount++], NULL, (void *)reader, (void *)&a[rcount-1]);
            }
            else if (scenario[i] == 'w')
            {
                pthread_create(&wlock[wcount++], NULL, (void *)writer, (void *)&a[wcount-1]);
            }
        }

        for (int i = 0; i < read; i++)
        {
            if (scenario[i] == 'r')
            {
                pthread_join(rlock[--rcount], NULL);
            }
            else if (scenario[i] == 'w')
            {
                pthread_join(wlock [--wcount], NULL);
            }
        }
        
        printf("\nSimulation Finished...\n=====\n");
        
    }

    free(scenario);
    deinit();
    exit(EXIT_SUCCESS);
}