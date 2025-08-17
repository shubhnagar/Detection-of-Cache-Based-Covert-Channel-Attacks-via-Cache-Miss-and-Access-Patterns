#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "./cacheutils.h"
#include <pthread.h>
#include <papi.h>

// RAJ


void secret_protocol(char *message, int message_len)
{
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("[Sender] open failed");
        exit(1);
    }

    // Allocate shared memory (only 1 int is needed)
    ftruncate(fd, FILESIZE);
    char *shared_mem = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("[Sender] mmap failed");
        exit(1);
    }
    // printf("1 message size: %d\n", message_len);

    unsigned char buff[16];
    int j = 0;
    int counter = 0;

    // printf("here\n");
    // printf("here\n");
    // printf("counter: %d\n", counter);
    while (j <= 40)
    {
        volatile char value;
        value = *(shared_mem + 129);
        j++;
        value = *(shared_mem + 65);
        sched_yield();
    }
    // printf("counter: %d\n", counter);
    unsigned long long target_cycles = CHANNEL_INTERVAL;
    // printf("start time: %lld\n", rdtsc());
    for (int i = 0; i < message_len * 8; ++i)
    {
        unsigned long long start = rdtsc();
        int j = 0;
        size_t start_time = rdtsc();
        while ((rdtsc() - start) < target_cycles)
        {
            if (message[i / 8] & (1 << (i % 8)))
            {
                volatile char value = *shared_mem;
            }
        }
        size_t end_time = rdtsc();
    }

    close(fd);
}

int main()
{
    printf("[Sender] sender started, PID: %d\n", getpid());

    // ********** DO NOT MODIFY THIS SECTION **********
    FILE *fp = fopen(MSG_FILE, "r");
    if (fp == NULL)
    {
        printf("[Sender] Error opening file\n");
        return 1;
    }

    char msg[MAX_MSG_SIZE];
    int msg_size = 0;
    char c;
    while ((c = fgetc(fp)) != EOF)
    {
        msg[msg_size++] = c;
    }
    fclose(fp);


    clock_t start = clock();
    // **********************************************
    // ********** YOUR CODE STARTS HERE **********

    secret_protocol(msg, msg_size);


    // ********** YOUR CODE ENDS HERE **********
    // ********** DO NOT MODIFY THIS SECTION **********
    clock_t end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("[Sender] Message sent successfully\n");
    printf("[Sender] Time taken to send the message: %f\n", time_taken);
    printf("[Sender] Message size: %d\n", msg_size);
    printf("[Sender] Bits per second: %f\n", msg_size * 8 / time_taken);
    // **********************************************
}
