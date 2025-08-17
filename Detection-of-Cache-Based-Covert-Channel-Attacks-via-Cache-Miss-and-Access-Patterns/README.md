folder /t1 was used to conduct experiments, inside it theres a load folder, which has the 3 types of scrits we used to perform load testing.
folder /t2 was used to play around while we approached the design of detector from different angles.
folder /t3 has the final detector code.

To compile any of the code that has papi Api you need to use "-lpapi" to link the library.

Before running any of the code that has papu api used in it do:
    `sudo sh -c 'echo 1 > /proc/sys/kernel/perf_event_paranoid'`

to run the detector in /t3: taskset -c <core_num> ./detector ./sender
to run the receiver in /t3: taskset -c <core_num> ./receiver
