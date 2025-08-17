#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <papi.h>
#include "./cacheutils.h"

int main(int argc, char *argv[])
{

    // --------------------- PAPI SETUP ------------------------

    int retval;
    int event_set = PAPI_NULL;
    long long values[3];

    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT)
    {
        fprintf(stderr, "[Detector] PAPI library init error: %s\n", PAPI_strerror(retval));
        exit(1);
    }

    if (PAPI_create_eventset(&event_set) != PAPI_OK)
    {
        fprintf(stderr, "[Detector] Error creating event set\n");
        exit(1);
    }

    if (PAPI_add_event(event_set, PAPI_L3_TCA) != PAPI_OK)
    {
        fprintf(stderr, "[Detector] Error adding event: %s\n", PAPI_strerror(PAPI_L3_TCA));
        exit(1);
    }
    if (PAPI_add_event(event_set, PAPI_L3_TCM) != PAPI_OK)
    {
        fprintf(stderr, "[Detector] Error adding event: %s\n", PAPI_strerror(PAPI_L3_TCA));
        exit(1);
    }

    if (PAPI_add_event(event_set, PAPI_TOT_INS) != PAPI_OK)
    {
        fprintf(stderr, "[Detector] Error adding event: %s\n", PAPI_strerror(PAPI_TOT_INS));
        exit(1);
    }

    // --------------------- PAPI SETUP ------------------------

    if (argc < 2)
    {
        fprintf(stderr, "[Detector] Usage: %s <program_to_run> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("[Detector] fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        execvp(argv[1], &argv[1]);
        perror("[Detector] exec failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        PAPI_attach(event_set, (unsigned long)pid);
        printf("[Detector] Started program '%s' with PID: %d\n", argv[1], pid);
        long long ca = 0;
        long long cm = 0;
        long long ins = 0;
        int status;

        long long start_time = rdtsc();

        while (1)
        {
            // --------------------- PAPI start ------------------------
            if (PAPI_start(event_set) != PAPI_OK)
            {
                fprintf(stderr, "[Detector] Error starting counters\n");
                exit(1);
            }
            // --------------------- PAPI start ------------------------
            usleep(10000);
            // --------------------- PAPI end ------------------------
            if (PAPI_stop(event_set, values) != PAPI_OK)
            {
                fprintf(stderr, "[Detector] Error stopping counters\n");
                exit(1);
            }
            // --------------------- PAPI end ------------------------
            ca += values[0];
            cm += values[1];
            ins += values[2];
            // printf("L3 Total Cache Accesses : %lld\n", ca);
            // printf("L3 Total Cache Misses : %lld\n", cm);
            // printf("L3 Total Cache ins : %lld\n", ins);

            // printf("here\n");

            if (ca > 100000) {
                printf("[Detector] Suspicious activity detected, killing process %d\n", pid);
                if (kill(pid, SIGTERM) == 0) {
                    printf("[Detector] Killed the suspect: %d\n", pid);
                } else {
                    perror("[Detector] Failed to send SIGTERM");
                }
                printf("[Detector] Clock cycles taken to detect: %lld\n", rdtsc() - start_time);
                break;
            }

            pid_t result = waitpid(pid, &status, WNOHANG);
            if (result == -1)
            {
                perror("waitpid");
                break;
            }
        }
        printf("[Detector] L3 Total Cache Accesses : %lld\n", ca);
        printf("[Detector] L3 Total Cache Misses : %lld\n", cm);
        printf("[Detector] L3 Total Cache ins : %lld\n", ins);

        // waitpid(pid, &status, 0);
        // printf("[Detector] Program exited with status: %d\n", WEXITSTATUS(status));
    }

    return 0;
}
