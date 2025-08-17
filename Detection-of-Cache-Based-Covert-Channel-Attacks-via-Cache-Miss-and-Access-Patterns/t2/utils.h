#include <stdio.h>

#define MSG_FILE "msg.txt"
#define MAX_MSG_SIZE 500

#define MESSAGE_SIZE 168 // 8 bits (1 byte)
#define CACHE_LINE_SIZE 64
#define FILESIZE 1024 * 10
#define FILE_NAME "./invisibleink"
#define THRESHOLD 295
#define CHANNEL_INTERVAL 1000000

double check_accuracy(char*, int);
