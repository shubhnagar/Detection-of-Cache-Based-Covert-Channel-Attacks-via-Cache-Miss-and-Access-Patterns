#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1024  

int main() {

    // --------------------- PAPI SETUP ------------------------

    int retval;
    int event_set = PAPI_NULL;
    long long values[2]; // Make room for 2 events

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
        fprintf(stderr, "Error adding L3_TCA event\n");
        exit(1);
    }

    if (PAPI_add_event(event_set, PAPI_L3_TCM) != PAPI_OK) {
        fprintf(stderr, "Error adding L3_TCM event\n");
        exit(1);
    }
    // ---------------------------------------------


    // --------------------- PAPI start ------------------------
    if (PAPI_start(event_set) != PAPI_OK) {
        fprintf(stderr, "Error starting counters\n");
        exit(1);
    }
    // ---------------------------------------------


    static double A[SIZE][SIZE];
    static double B[SIZE][SIZE];
    static double C[SIZE][SIZE];

    // Fill matrices A and B with random values
    srand(time(NULL));
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j) {
            A[i][j] = rand() % 1000;
            B[i][j] = rand() % 1000;
        }

    // Perform matrix multiplication: C = A * B
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            for (int k = 0; k < SIZE; ++k)
                C[i][j] += A[i][k] * B[k][j];

    printf("Matrix multiplication done for size %d x %d\n", SIZE, SIZE);

    // --------------------- PAPI end ------------------------
    if (PAPI_stop(event_set, values) != PAPI_OK) {
        fprintf(stderr, "Error stopping counters\n");
        exit(1);
    }
    // ---------------------------------------------

    printf("L3 Total Cache Accesses: %lld\n", values[0]);
    printf("L3 Total Cache Misses : %lld\n", values[1]);

    return 0;
}
