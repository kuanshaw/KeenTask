#ifndef __KUAN_RTOS_H
#define __KUAN_RTOS_H

#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "rtthread.h"


#define KTASK_NAME_MAX          8
#define KTASK_TICK_INTERVAL     1           //1ms


#define KTASK_FLAG_CLOSE        0x00
#define KTASK_FLAG_OPEN         0x01
#define KTASK_FLAG_SUSPEND      0x02
#define _KTASK_FLAG_RUN         0x10

#define _K_MEMCPY            memcpy
#define _K_MEMSET            memset 
#define _K_PRINTF(fmt,...)   rt_kprintf("[ktask]: "fmt"\r\n",##__VA_ARGS__)


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


/** 
 * @brief      创建任务
 * @param[in]  obj 任务控制块
 * @param[in]  name 任务名字 长度不能超过KTASK_NAME_MAX个字节
 * @param[in]  new_task 任务函数
 * @param[in]  period_ms 任务执行周期 单位为毫秒
 * @param[in]  priority 任务优先级 同执行周期下 越大越先执行
 * @param[in]  arg 参数
 * @retval     
 * @note       
 */ 
ktask_obj_t *ktask_create(ktask_obj_t *obj, const char *name, ktask_ptr new_task, uint32_t period_ms, uint8_t priority, void *arg);


/** 
 * @brief      开始任务
 * @param[in]  obj 任务控制块
 * @retval     0-成功 -1-参数错误
 * @note       
 */ 
int8_t ktask_start(ktask_obj_t *obj);


/** 
 * @brief      删除任务
 * @param[in]  obj 任务控制块
 * @retval     0-成功 -1-参数错误 -2-任务未初始化
 * @note       
 */ 
int8_t ktask_delete(ktask_obj_t *obj);


/** 
 * @brief      挂起任务
 * @param[in]  obj 任务控制块
 * @retval     0-成功 -1-参数错误 -2-任务未初始化
 * @note       
 */ 
int8_t ktask_suspend(ktask_obj_t *obj);


/** 
 * @brief      恢复任务
 * @param[in]  obj 任务控制块
 * @retval     0-成功 -1-参数错误 -2-任务未初始化
 * @note       
 */ 
int8_t ktask_resume(ktask_obj_t *obj);


/** 
 * @brief      打印当前任务
 * @param      无
 * @retval     无
 * @note       
 */ 
void ktask_list(void);


/** 
 * @brief      任务调度函数
 * @param      无
 * @retval     无
 * @note       
 */ 
void ktask_schedule(void);


/** 
 * @brief      任务滴答函数
 * @param      无
 * @retval     无
 * @note       
 */ 
void ktask_tick(uint16_t period);


/** 
 * @brief      任务错误
 * @param      无
 * @retval     无
 * @note       
 */ 
void ktask_assert(void);










#endif 

