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
#include <papi.h>


// RAJ

void secret_protocol(char *message, int message_len)
{
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("open failed");
        exit(1);
    }

    // Allocate shared memory (only 1 int is needed)
    ftruncate(fd, FILESIZE);
    char *shared_mem = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }
    // printf("1 message size: %d\n", message_len);

    unsigned char buff[16];
    int j = 0;
    int counter = 0;

    // printf("here\n");
    // printf("here\n");
    // printf("counter: %d\n", counter);
    // --------------------- PAPI SETUP ------------------------

     int retval;
     int event_set = PAPI_NULL;
     long long values[3];
 
     retval = PAPI_library_init(PAPI_VER_CURRENT);
     if (retval != PAPI_VER_CURRENT) {
         fprintf(stderr, "PAPI library init error: %s\n", PAPI_strerror(retval));
         exit(1);
     }
 
     if (PAPI_create_eventset(&event_set) != PAPI_OK) {
         fprintf(stderr, "Error creating event set\n");
         exit(1);
     }
 
     if (PAPI_add_event(event_set, PAPI_L3_TCA) != PAPI_OK) {
         fprintf(stderr, "Error adding event: %s\n", PAPI_strerror(PAPI_L3_TCA));
         exit(1);
     }
     if (PAPI_add_event(event_set, PAPI_L3_TCM) != PAPI_OK) {
        fprintf(stderr, "Error adding event: %s\n", PAPI_strerror(PAPI_L3_TCM));
        exit(1);
    }
    if (PAPI_add_event(event_set, PAPI_TOT_INS) != PAPI_OK)
    {
        fprintf(stderr, "Error adding event: %s\n", PAPI_strerror(PAPI_TOT_INS));
        exit(1);
    }
    // --------------------- PAPI SETUP ------------------------


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
    // printf("time: %lld\n", rdtsc());
    // --------------------- PAPI start ------------------------
    if (PAPI_start(event_set) != PAPI_OK) {
        fprintf(stderr, "Error starting counters\n");
        exit(1);
    }
    // --------------------- PAPI start ------------------------
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
    // --------------------- PAPI end ------------------------
    if (PAPI_stop(event_set, values) != PAPI_OK) {
        fprintf(stderr, "Error stopping counters\n");
        exit(1);
    }
    // --------------------- PAPI end ------------------------
    printf("%lld %lld %lld\n", values[0], values[1], values[2]);
    // printf("L3 Total Cache Accesses : %lld\n", values[0]);
    // printf("L3 Total Cache Misses : %lld\n", values[1]);
    close(fd);
}

int main()
{

    // ********** DO NOT MODIFY THIS SECTION **********
    FILE *fp = fopen(MSG_FILE, "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
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
    // printf("Message sent successfully\n");
    // printf("Time taken to send the message: %f\n", time_taken);
    // printf("Message size: %d\n", msg_size);
    // printf("Bits per second: %f\n", msg_size * 8 / time_taken);
    // **********************************************
}
