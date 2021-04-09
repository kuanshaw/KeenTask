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
 * @brief      ��������
 * @param[in]  obj ������ƿ�
 * @param[in]  name �������� ���Ȳ��ܳ���KTASK_NAME_MAX���ֽ�
 * @param[in]  new_task ������
 * @param[in]  period_ms ����ִ������ ��λΪ����
 * @param[in]  priority �������ȼ� ִͬ�������� Խ��Խ��ִ��
 * @param[in]  arg ����
 * @retval     
 * @note       
 */ 
ktask_obj_t *ktask_create(ktask_obj_t *obj, const char *name, ktask_ptr new_task, uint32_t period_ms, uint8_t priority, void *arg);


/** 
 * @brief      ��ʼ����
 * @param[in]  obj ������ƿ�
 * @retval     0-�ɹ� -1-��������
 * @note       
 */ 
int8_t ktask_start(ktask_obj_t *obj);


/** 
 * @brief      ɾ������
 * @param[in]  obj ������ƿ�
 * @retval     0-�ɹ� -1-�������� -2-����δ��ʼ��
 * @note       
 */ 
int8_t ktask_delete(ktask_obj_t *obj);


/** 
 * @brief      ��������
 * @param[in]  obj ������ƿ�
 * @retval     0-�ɹ� -1-�������� -2-����δ��ʼ��
 * @note       
 */ 
int8_t ktask_suspend(ktask_obj_t *obj);


/** 
 * @brief      �ָ�����
 * @param[in]  obj ������ƿ�
 * @retval     0-�ɹ� -1-�������� -2-����δ��ʼ��
 * @note       
 */ 
int8_t ktask_resume(ktask_obj_t *obj);


/** 
 * @brief      ��ӡ��ǰ����
 * @param      ��
 * @retval     ��
 * @note       
 */ 
void ktask_list(void);


/** 
 * @brief      ������Ⱥ���
 * @param      ��
 * @retval     ��
 * @note       
 */ 
void ktask_schedule(void);


/** 
 * @brief      ����δ���
 * @param      ��
 * @retval     ��
 * @note       
 */ 
void ktask_tick(uint16_t period);


/** 
 * @brief      �������
 * @param      ��
 * @retval     ��
 * @note       
 */ 
void ktask_assert(void);










#endif 

