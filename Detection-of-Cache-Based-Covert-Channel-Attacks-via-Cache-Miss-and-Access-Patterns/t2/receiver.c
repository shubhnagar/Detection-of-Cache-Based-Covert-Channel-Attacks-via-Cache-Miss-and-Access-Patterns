#include <stdio.h>
#include "./utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "./cacheutils.h"
#include "./utils.c"


// SIMRAN
void receive_bit_by_bit(char *received_msg, int message_len)
{
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("open failed");
        exit(1);
    }
    ftruncate(fd, FILESIZE);
    char *shared_mem = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }
    int j = 0;
    int counter = 0;
    while (j <= 20)
    {
        size_t start_time = rdtsc();
        volatile char value;
        value = *(shared_mem + 65);
        size_t end_time = rdtsc();
        counter++;
        if (end_time - start_time < THRESHOLD)
        {
        j++;
        }
        flush((shared_mem + 65));
        sched_yield();
    }

    printf("counter: %d\n", counter);
    unsigned long long target_cycles = CHANNEL_INTERVAL;

    unsigned char buff[16];
    // usleep(30);
    printf("time: %lld\n", rdtsc());
    for (int i = 0; i < message_len * 8; ++i)
    {   
        unsigned long long start = rdtsc();
        
        int hits = 0, misses = 0;
        size_t start_time1 = rdtsc();
        j = 0;
        while ((rdtsc() - start) < target_cycles)
        {
            size_t start_time = rdtsc();
            volatile char value = *shared_mem;
            size_t end_time = rdtsc();
            if (end_time - start_time < THRESHOLD)
            {
                hits++;// Set bit to 1
            } else {
                misses++;
            }
            flush(shared_mem);
            
        }
        
        int bit_position = i % 8;
        int byte_index = i / 8;
        if (hits > misses) {
            received_msg[byte_index] |= (1 << bit_position);
        }
        size_t end_time1 = rdtsc();
    }
    close(fd);
}

int main()
{

    // Update these values accordingly
    char *received_msg = NULL;
    int received_msg_size = 0;

    received_msg = (char *)malloc(MESSAGE_SIZE + 1);
    memset(received_msg, 0, MESSAGE_SIZE + 1);

    char *received_msg_temp = (char *)malloc(MESSAGE_SIZE + 1);
    memset(received_msg_temp, 0, MESSAGE_SIZE + 1);

    receive_bit_by_bit(received_msg_temp, MESSAGE_SIZE);

    printf("Received message: \n");
    for (int i = 0; i < MESSAGE_SIZE + 1; i++)
    {
        printf("%c", received_msg_temp[i]);
        if (received_msg_temp[i] != '\0')
        {
            received_msg[i] = received_msg_temp[i];
        }
        else
        {
            received_msg[i] = '_';
        }
    }
    printf("Received message: %s\n", received_msg);

    // DO NOT MODIFY THIS LINE
    printf("Accuracy (%%): %f\n", check_accuracy(received_msg, strlen(received_msg)) * 100);
}
