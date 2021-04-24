#ifndef __KEEN_TASK_H
#define __KEEN_TASK_H

#include <stdint.h>

#define _KTASK_DEBUG
#define KTASK_NAME_MAX          8

#include <string.h>
#define _K_MEMCPY(dec, src, size)           memcpy(dec, src, size)
#define _K_MEMSET(str, ch, size)            memset(str, ch, size)

#ifdef _KTASK_DEBUG
#include <stdio.h>
#define _K_PRINTF(fmt,...)                  printf("[ktask]: "fmt"\r\n",##__VA_ARGS__)
#else
#define _K_PRINTF(fmt,...) 
#endif

#define KTASK_FLAG_CLOSE        0x00
#define KTASK_FLAG_OPEN         0x01
#define KTASK_FLAG_SUSPEND      0x02
#define KTASK_FLAG_READY        0x10

typedef void (*ktask_ptr)(void *arg);
typedef struct ktask_obj ktask_obj_t;

/** 
 * 任务对象结构体
 */ 
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
 * @param[in]  obj 任务对象
 * @param[in]  name 任务名字，长度不能超过KTASK_NAME_MAX个字节
 * @param[in]  new_task 任务函数
 * @param[in]  period_ms 任务执行周期，单位为毫秒
 * @param[in]  priority 任务优先级，同执行周期下，优先级值越小越先执行
 * @param[in]  arg 用户参数，需要在外部申请存储空间
 * @retval     任务对象
 * @note       
 */ 
ktask_obj_t *ktask_create(ktask_obj_t *obj, const char *name, ktask_ptr new_task, uint32_t period_ms, uint8_t priority, void *arg);


/** 
 * @brief      开始任务
 * @param[in]  obj 任务对象
 * @retval     0-成功
 * @note       
 */ 
int8_t ktask_start(ktask_obj_t *obj);


/** 
 * @brief      删除任务
 * @param[in]  obj 任务对象
 * @retval     0-成功
 * @note       
 */ 
int8_t ktask_delete(ktask_obj_t *obj);


/** 
 * @brief      挂起任务
 * @param[in]  obj 任务对象
 * @retval     0-成功
 * @note       
 */ 
int8_t ktask_suspend(ktask_obj_t *obj);


/** 
 * @brief      恢复任务
 * @param[in]  obj 任务对象
 * @retval     0-成功
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



#endif 
