#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int findPageIndex(unsigned page, unsigned pages[], int frameNumber)
{
    int res = -1;
    for (int index = 0; index < frameNumber; index++)
    {
        if (pages[index] == page)
        {
            res = index;
            break;
        }
    }
    return res;
}

bool isBufferAvailable(unsigned page, unsigned pages[], int size)
{
    if (size == 0)
    {
        return false;
    }
    bool res = false;
    for (int index = 0; index < size; index++)
    {
        if (pages[index] == 0)
        {
            res = true;
            break;
        }
    }
    return res;
}

void insertPage(unsigned page, char rw, unsigned pages[], char rws[], int frameNumber)
{

    for (int index = 0; index < frameNumber; index++)
    {
        if (pages[index] == 0)
        {
            pages[index] = page;
            rws[index] = rw;
            break;
        }
    }
}

void flushArray(unsigned pages[], int frameNumber)
{
    // printf("Flushing\n");
    for (int index = 0; index < frameNumber; index++)
    {
        pages[index] = 0;
        // printf("%x ", pages[index]);
    }
    // printf("\nEnd flushing\n");
}

void printOutput(int nFrames, int events, int read, int write, int pageFault)
{
    printf("\n=========\n");
    printf("total memory frames: %d\n", nFrames);
    printf("events in trace: %d\n", events);
    printf("total disk reads: %d\n", read);
    printf("total disk writes: %d\n", write);
    printf("total page faults: %d\n", pageFault);
}

void lru(FILE *fp, int nFrames, bool isDebug)
{
    int events = 0;
    unsigned page;
    unsigned pages[nFrames];
    char rw;
    char rws[nFrames];
    int counter[nFrames];
    int read = 0;
    int write = 0;
    int pageFault = 0;
    int currentSize = 0;
    int maxCounterIndex = 0;
    int max = 0;
    flushArray(pages, nFrames);
    unsigned convertedPage;
    while (fscanf(fp, "%x %c", &page, &rw) != EOF)
    {
        convertedPage = page >> 12;
        if (isDebug)
            printf("Adding Page %x %c\n", convertedPage, rw);
        if (currentSize < nFrames)
        {
            int res = findPageIndex(convertedPage, pages, nFrames);
            if (res == -1)
            {
                // insertPage(page, rw, pages, rws, frameNumber);
                for (int index = 0; index < nFrames; index++)
                {
                    if (pages[index] != 0)
                    {
                        counter[index]++;
                    }
                    else if (pages[index] == 0)
                    {
                        pages[index] = convertedPage;
                        rws[index] = rw;
                        counter[index] = 1;
                        read++;
                        pageFault++;
                        break;
                    }
                }
                currentSize++;
            }
            else
            {
                for (int index = 0; index < nFrames; index++)
                {
                    if (index == res)
                    {
                        counter[index] = 1;
                    }
                    else
                    {
                        counter[index]++;
                    }
                }
            }
        }
        else
        {
            int res = findPageIndex(convertedPage, pages, nFrames);
            if (res == -1)
            {
                read++;
                maxCounterIndex = 0;
                max = 0;
                for (int index = 0; index < nFrames; index++)
                {
                    if (counter[index] > max)
                    {
                        max = counter[index];
                        maxCounterIndex = index;
                    }
                }

                if (rws[maxCounterIndex] == 'W')
                {
                    write++;
                }
                for (int index = 0; index < nFrames; index++)
                {
                    if (index != maxCounterIndex)
                    {
                        counter[index]++;
                    }
                    else
                    {
                        pages[index] = convertedPage;
                        rws[index] = rw;
                        counter[index] = 1;
                    }
                }
                pageFault++;
            }
            else
            {
                for (int index = 0; index < nFrames; index++)
                {
                    if (index == res)
                    {
                        counter[index] = 1;
                    }
                    else
                    {
                        counter[index]++;
                    }
                }
            }
        }
        events++;
    }
    printOutput(nFrames, events, read, write, pageFault);
}

void segmented_fifo(FILE *fp, int nFrames, int p, bool isDebug)
{
    int primarySize = nFrames / 100 * (100 - p);
    int secondarySize = nFrames - primarySize;
    int events = 0;
    unsigned page;
    unsigned primaryBuffer[primarySize];
    unsigned secondaryBuffer[secondarySize];
    char rw;
    char primaryRws[primarySize];
    char secondaryRws[secondarySize];
    int counter[secondarySize];
    int read = 0;
    int write = 0;
    int pageFault = 0;
    int currentSize = 0;
    int maxCounterIndex = 0;
    int max = 0;
    flushArray(primaryBuffer, primarySize);
    flushArray(secondaryBuffer, secondarySize);
    unsigned convertedPage;
    unsigned poppingPage;
    unsigned poppingRw;

    while (fscanf(fp, "%x %c", &page, &rw) != EOF)
    {
        convertedPage = page >> 12;
        if (isDebug)
            printf("Adding Page %x %c\n", convertedPage, rw);
        // When primary is not full
        if (isBufferAvailable(convertedPage, primaryBuffer, primarySize))
        {
            int res = findPageIndex(convertedPage, primaryBuffer, primarySize);
            if (res == -1)
            {
                insertPage(convertedPage, rw, primaryBuffer, primaryRws, primarySize);
                read++;
                pageFault++;
            }
        }
        else
        {
            // When primary is full but secondary is not
            if (isBufferAvailable(convertedPage, secondaryBuffer, secondarySize))
            {
                poppingPage = primaryBuffer[0];
                poppingRw = primaryRws[0];

                for (int i = 1; i < primarySize; i++)
                {
                    primaryBuffer[i - 1] = primaryBuffer[i];
                    primaryRws[i - 1] = primaryRws[i];
                }
                primaryBuffer[primarySize - 1] = convertedPage;
                primaryRws[primarySize - 1] = rw;

                int res = findPageIndex(poppingPage, secondaryBuffer, secondarySize);
                if (res == -1)
                {
                    // Move popping stuffs to secondary
                    for (int index = 0; index < secondarySize; index++)
                    {
                        if (secondaryBuffer[index] != 0)
                        {
                            counter[index]++;
                        }
                        else if (secondaryBuffer[index] == 0)
                        {
                            secondaryBuffer[index] = poppingPage;
                            secondaryRws[index] = poppingRw;
                            counter[index] = 1;
                        }
                    }
                    read++;
                    pageFault++;
                    if (poppingRw == 'W')
                    {
                        write++;
                    }
                }
                else
                {
                    for (int index = 0; index < secondarySize; index++)
                    {
                        if (secondaryBuffer[index] != 0)
                        {
                            counter[index]++;
                        }
                        else if (secondaryBuffer[index] == poppingPage)
                        {
                            counter[index] = 1;
                            break;
                        }
                    }
                }
            }
            else // WHen both buffer is full
            {
                int res = findPageIndex(convertedPage, primaryBuffer, primarySize);
                if (res == -1)
                {
                    // if (secondary Buffer doesn't have the page loaded)
                    res = findPageIndex(convertedPage, secondaryBuffer, secondarySize);
                    if (res == -1)
                    {
                        if (primarySize > 0)
                        {
                            poppingPage = primaryBuffer[0];
                            poppingRw = primaryRws[0];
                        }
                        else
                        {
                            poppingPage = convertedPage;
                            poppingRw = rw;
                        }

                        for (int i = 1; i < primarySize; i++)
                        {
                            primaryBuffer[i - 1] = primaryBuffer[i];
                            primaryRws[i - 1] = primaryRws[i];
                        }
                        primaryBuffer[primarySize - 1] = convertedPage;
                        primaryRws[primarySize - 1] = rw;

                        maxCounterIndex = 0;
                        max = 0;
                        for (int index = 0; index < secondarySize; index++)
                        {
                            if (counter[index] > max)
                            {
                                max = counter[index];
                                maxCounterIndex = index;
                            }
                        }
                        for (int index = 0; index < secondarySize; index++)
                        {
                            if (index != maxCounterIndex)
                            {
                                counter[index]++;
                            }
                            else
                            {
                                secondaryBuffer[index] = poppingPage;
                                secondaryRws[index] = poppingRw;
                                counter[index] = 1;
                            }
                        }
                        read++;
                        pageFault++;
                        if (poppingRw == 'W')
                        {
                            write++;
                        }
                    }
                    else // When the new page is already in loaded in secondary buffer
                    {
                        poppingPage = secondaryBuffer[res];
                        poppingRw = secondaryRws[res];
                        if (primarySize > 0)
                        {
                            secondaryBuffer[res] = primaryBuffer[0];
                            secondaryRws[res] = primaryRws[0];
                            counter[res] = 1;
                            for (int i = 0; i < secondarySize; i++)
                            {
                                if (i != res)
                                {
                                    counter[i]++;
                                }
                            }
                            for (int i = 1; i < primarySize; i++)
                            {
                                primaryBuffer[i - 1] = primaryBuffer[i];
                                primaryRws[i - 1] = primaryRws[i];
                            }
                            primaryBuffer[primarySize - 1] = poppingPage;
                            primaryRws[primarySize - 1] = poppingRw;
                            read++;
                            pageFault++;
                            if (poppingRw == 'W')
                            {
                                write++;
                            }
                        }
                        else
                        {
                            for (int index = 0; index < secondarySize; index++)
                            {
                                if (index != res)
                                {
                                    counter[index]++;
                                }
                                else
                                {
                                    counter[index] = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
        events++;
    }
    printOutput(nFrames, events, read, write, pageFault);
}

void fifo(FILE *fp, int nFrames, bool isDebug)
{
    int events = 0;
    unsigned page;
    unsigned pages[nFrames];
    char rw;
    char rws[nFrames];

    int read = 0;
    int write = 0;
    int pageFault = 0;
    int currentSize = 0;
    flushArray(pages, nFrames);
    unsigned convertedPage;

    while (fscanf(fp, "%x %c", &page, &rw) != EOF)
    {
        convertedPage = page >> 12;
        if (isDebug)
            printf("Adding Page %x %c\n", convertedPage, rw);
        if (currentSize < nFrames)
        {
            int res = findPageIndex(convertedPage, pages, nFrames);
            if (res == -1)
            {
                // insertPage(page, rw, pages, rws, frameNumber);
                for (int index = 0; index < nFrames; index++)
                {
                    if (pages[index] == 0)
                    {
                        pages[index] = convertedPage;
                        rws[index] = rw;
                        read++;
                        pageFault++;
                        break;
                    }
                }
                currentSize++;
            }
        }
        else
        {
            int res = findPageIndex(convertedPage, pages, nFrames);
            if (res == -1)
            {
                read++;
                if (rws[0] == 'W')
                {
                    write++;
                }
                for (int i = 1; i < nFrames; i++)
                {
                    pages[i - 1] = pages[i];
                    rws[i - 1] = rws[i];
                }
                pages[nFrames - 1] = convertedPage;
                rws[nFrames - 1] = rw;
                pageFault++;
            }
            else
            {
                if (rw == 'W' && rws[res] == 'R')
                    write++;
            }
        }

        events++;
    }
    printOutput(nFrames, events, read, write, pageFault);
}

int main(int argc, char *argv[])
{
    char *traceFile = argv[1]; // "bzip.trace";
    int nFrames = atoi(argv[2]);
    char *algo = argv[3]; //"lru";        //lru,fifo,vms
    int p;
    char *outputMode;
    if (strcmp(algo, "vms") == 0)
    {
        p = atoi(argv[4]);
        outputMode = argv[5]; //debug/quite
    }
    else
    {
        outputMode = argv[4]; //debug/quite
    }

    bool isDebug = strcmp(outputMode, "debug") == 0;

    FILE *fp;
    fp = fopen(traceFile, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    /***************/

    if (strcmp(algo, "fifo") == 0)
    {
        fifo(fp, nFrames, isDebug);
    }
    else if (strcmp(algo, "lru") == 0)
    {
        lru(fp, nFrames, isDebug);
    }
    else
    {
        segmented_fifo(fp, nFrames, p, isDebug);
    }
    // for (int i = 0; i < nFrames; i++)
    // {
    //     printf("%x %c\n", pages[i], rws[i]);
    // }
    /***************/
    fclose(fp);
    return 0;
}
