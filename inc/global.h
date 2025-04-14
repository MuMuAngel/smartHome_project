#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "control.h"
#include <mqueue.h>
typedef struct{
    mqd_t mq;
    struct control *cntl_phead;
}cntl_info_t;

#endif