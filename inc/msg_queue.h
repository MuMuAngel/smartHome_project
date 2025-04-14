#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <pthread.h>
#include <errno.h>

#define MAX_MSG_SIZE 1024
#define MAX_MSG_NUM 10
#define QUEUE_NAME "/msg_queue"

mqd_t msg_queue_init(void);
int msg_queue_send(mqd_t mqd, void *msg, size_t msg_len);
void msg_queue_destroy(mqd_t mqd);

#endif