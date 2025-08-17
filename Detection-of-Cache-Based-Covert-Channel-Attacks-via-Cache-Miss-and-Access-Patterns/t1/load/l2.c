#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>
#include <unistd.h>

#define BUFFER_SIZE (64 * 1024 * 1024) // 64MB
#define ITERATIONS 100000000

int main() {
    char *buffer = (char *)malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("malloc failed");
        return 1;
    }

    // Initialize buffer
    memset(buffer, 1, BUFFER_SIZE);

    // PAPI setup
    // int event_set = PAPI_NULL;
    // long long values[2];

     // --------------------- PAPI SETUP ------------------------

     int retval;
     int event_set = PAPI_NULL;
     long long values[2];
 
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
        fprintf(stderr, "Error adding event: %s\n", PAPI_strerror(PAPI_L3_TCA));
        exit(1);
    }
     // --------------------- PAPI SETUP ------------------------
     // --------------------- PAPI start ------------------------
     if (PAPI_start(event_set) != PAPI_OK) {
        fprintf(stderr, "Error starting counters\n");
        exit(1);
    }
    // --------------------- PAPI start ------------------------
    
    // Heavy cache accesses
    for (int it = 0; it < ITERATIONS; it++) {
        for (int i = 0; i < BUFFER_SIZE; i += 64) {
            buffer[i]++;
        }
    }
    
    printf("here\n");
    // --------------------- PAPI end ------------------------
    if (PAPI_stop(event_set, values) != PAPI_OK) {
        fprintf(stderr, "Error stopping counters\n");
        exit(1);
    }
    // --------------------- PAPI end ------------------------

    printf("L3 Total Cache Accesses: %lld\n", values[0]);
    printf("L3 Total Cache Misses : %lld\n", values[1]);

    free(buffer);
    return 0;
}
