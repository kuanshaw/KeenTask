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
 * �������ṹ��
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
 * @brief      ��������
 * @param[in]  obj �������
 * @param[in]  name �������֣����Ȳ��ܳ���KTASK_NAME_MAX���ֽ�
 * @param[in]  new_task ������
 * @param[in]  period_ms ����ִ�����ڣ���λΪ����
 * @param[in]  priority �������ȼ���ִͬ�������£����ȼ�ֵԽСԽ��ִ��
 * @param[in]  arg �û���������Ҫ���ⲿ����洢�ռ�
 * @retval     �������
 * @note       
 */ 
ktask_obj_t *ktask_create(ktask_obj_t *obj, const char *name, ktask_ptr new_task, uint32_t period_ms, uint8_t priority, void *arg);


/** 
 * @brief      ��ʼ����
 * @param[in]  obj �������
 * @retval     0-�ɹ�
 * @note       
 */ 
int8_t ktask_start(ktask_obj_t *obj);


/** 
 * @brief      ɾ������
 * @param[in]  obj �������
 * @retval     0-�ɹ�
 * @note       
 */ 
int8_t ktask_delete(ktask_obj_t *obj);


/** 
 * @brief      ��������
 * @param[in]  obj �������
 * @retval     0-�ɹ�
 * @note       
 */ 
int8_t ktask_suspend(ktask_obj_t *obj);


/** 
 * @brief      �ָ�����
 * @param[in]  obj �������
 * @retval     0-�ɹ�
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



#endif 
