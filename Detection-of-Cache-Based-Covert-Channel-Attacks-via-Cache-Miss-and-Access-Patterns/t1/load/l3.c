#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRSIZE (64 * 1024 * 1024)

int terminate = 0;

void handle_signal(int sig) {
    printf("\nCaught signal %d, cleaning up...\n", sig);

    terminate = 1;
    
}

int main() {
    int *arr = (int *) malloc(ARRSIZE * sizeof(int));
    signal(SIGINT, handle_signal);
    
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
    while (!terminate) {
        for (int i = 0; i < ARRSIZE; i+=16) {
            arr[i]++;
        }
    }
    

    // --------------------- PAPI end ------------------------
    if (PAPI_stop(event_set, values) != PAPI_OK) {
        fprintf(stderr, "Error stopping counters\n");
        exit(1);
    }
    // --------------------- PAPI end ------------------------
    printf("L3 Total Cache Accesses: %lld\n", values[0]);
    printf("L3 Total Cache Misses : %lld\n", values[1]);
    free(arr);
    return 0;
}