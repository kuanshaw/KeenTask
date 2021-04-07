#ifndef __KUAN_RTOS_H
#define __KUAN_RTOS_H

#include "stdint.h"
#include "string.h"
#include "stdio.h"

#define KTASK_NAME_MAX          8
#define KTASK_TICK_INTERVAL     1           //1ms


#define KTASK_FLAG_CLOSE        0x00
#define KTASK_FLAG_OPEN         0x01
#define KTASK_FLAG_SUSPEND      0x02
#define _KTASK_FLAG_RUN         0x10

#define _K_MEMCPY            memcpy
#define _K_MEMSET            memset 
#define _K_PRINTF(fmt,...)   printf("[ktask]: "fmt"\r\n",##__VA_ARGS__)




typedef void (*ktask_ptr)(void *arg);

typedef struct ktask_obj ktask_obj_t;

struct ktask_obj
{
    char name[KTASK_NAME_MAX];
    uint16_t period_ms;
    uint16_t tick;
    uint8_t flag;
    uint8_t priority;
    ktask_ptr task;

    void *arg;

    ktask_obj_t *next;
};

















#endif 

