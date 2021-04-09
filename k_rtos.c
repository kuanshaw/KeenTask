#include "k_rtos.h"

/** 
 * �δ����ֵ   
 */ 
static uint16_t _tick = 0;

/** 
 * �������ִ������
 */ 
static uint16_t _period_max = 0;

/** 
 * ������ͷ���
 */ 
static ktask_obj_t _head = 
{
    .name = "head",
    .next = NULL,
};


static void _reset_obj(ktask_obj_t *obj);
static void _update_period_max(void);
static int8_t _add_obj(ktask_obj_t *new_obj);
static int8_t _delete_obj(ktask_obj_t *tar_obj);


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
ktask_obj_t * ktask_create(ktask_obj_t *obj, const char *name, ktask_ptr new_task, uint32_t period_ms, uint8_t priority, void *arg)
{
    if((obj == NULL) || (name == NULL) || (new_task == NULL))
        return NULL; 

    _reset_obj(obj);

    _K_MEMCPY(obj->name, name, KTASK_NAME_MAX);
    obj->period_ms = period_ms;
    obj->flag |= KTASK_FLAG_OPEN;
    obj->priority = priority;
    obj->task = new_task;
    obj->arg = arg;
    
    return obj;
}


/** 
 * @brief      ��ʼ����
 * @param[in]  obj ������ƿ�
 * @retval     0-�ɹ� -1-��������
 * @note       
 */ 
int8_t ktask_start(ktask_obj_t *obj)
{
    return _add_obj(obj);
}


/** 
 * @brief      ɾ������
 * @param[in]  obj ������ƿ�
 * @retval     0-�ɹ� -1-�������� -2-����δ��ʼ��
 * @note       
 */ 
int8_t ktask_delete(ktask_obj_t *obj)
{
    if(obj == NULL)
        return -1; 
        
    if(!(obj->flag & KTASK_FLAG_OPEN))
        return -2;

    obj->flag = KTASK_FLAG_CLOSE;

    return _delete_obj(obj);
}


/** 
 * @brief      ��������
 * @param[in]  obj ������ƿ�
 * @retval     0-�ɹ� -1-�������� -2-����δ��ʼ��
 * @note       
 */ 
int8_t ktask_suspend(ktask_obj_t *obj)
{
    if(obj == NULL)
        return -1; 
        
    if(!(obj->flag & KTASK_FLAG_OPEN))
        return -2;

    obj->flag |= KTASK_FLAG_SUSPEND;

    _update_period_max();

    return 0;
}


/** 
 * @brief      �ָ�����
 * @param[in]  obj ������ƿ�
 * @retval     0-�ɹ� -1-�������� -2-����δ��ʼ��
 * @note       
 */ 
int8_t ktask_resume(ktask_obj_t *obj)
{
    if(obj == NULL)
        return -1; 
        
    if(!(obj->flag & KTASK_FLAG_OPEN))
        return -2;

    obj->flag &= (~KTASK_FLAG_SUSPEND);

    return 0;
}


/** 
 * @brief      ��ӡ��ǰ����
 * @param      ��
 * @retval     ��
 * @note       
 */ 
void ktask_list(void)
{
    ktask_obj_t *_obj = _head.next;

    while(_obj)
    {
        _K_PRINTF("Name: %s, Period: %d, Priority: %d, State: 0x%02X", _obj->name, \
                    _obj->period_ms, _obj->priority, _obj->flag);

        _obj = _obj->next;
    }
}


/** 
 * @brief      ������Ⱥ���
 * @param      ��
 * @retval     ��
 * @note       
 */ 
void ktask_schedule(void)
{
    ktask_obj_t *_obj = _head.next;

    while(_obj)
    {
        if(_obj->flag & _KTASK_FLAG_RUN)
        {
            _obj->task(_obj->arg);
            _obj->flag &= (~_KTASK_FLAG_RUN);
        }

        _obj = _obj->next;
    }
}


/** 
 * @brief      ����δ���
 * @param      ��
 * @retval     ��
 * @note       
 */ 
void ktask_tick(uint16_t period)
{
    ktask_obj_t *_obj = _head.next;
    uint16_t _tick_max = _period_max / period;
    _tick++;

    while(_obj)
    {
        _obj->tick = _obj->period_ms / period;
        
        if(_tick_max < _obj->tick)
            _tick_max = _obj->tick;

        if(((_tick % _obj->tick) == 0) && (!(_obj->flag & KTASK_FLAG_SUSPEND)))
            _obj->flag |= _KTASK_FLAG_RUN;

        _obj = _obj->next;
    }

    if(_tick >= _tick_max)
        _tick = 0;
}


/** 
 * @brief      �������
 * @param      ��
 * @retval     ��
 * @note       
 */ 
void ktask_assert(void)
{
    _K_PRINTF("assert!!!");
    while(1);
}


/** 
 * @brief      ������ƿ��ʼ��
 * @param[in]  obj ������ƿ�
 * @retval     ��
 * @note       
 */ 
static void _reset_obj(ktask_obj_t *obj)
{
    _K_MEMSET(obj->name, '\0', KTASK_NAME_MAX);
    obj->tick = 0;
    obj->flag = 0;
    obj->priority = 0;
    obj->task = NULL;
}


/** 
 * @brief      ���µ�ǰ����������������
 * @param      ��
 * @retval     ��
 * @note       
 */ 
static void _update_period_max(void)
{
    ktask_obj_t *_obj = _head.next;

    _period_max = 0;
    while(_obj)
    {
        if((_period_max < _obj->period_ms) && (!(_obj->flag & KTASK_FLAG_SUSPEND)))
            _period_max = _obj->period_ms;

        _obj = _obj->next;
    }

    _K_PRINTF("Update Max Period: %d", _period_max);
}


/** 
 * @brief      ���������
 * @param[in]  new_obj �������������ƿ�
 * @retval     0-�ɹ� -1-��������
 * @note       
 */ 
static int8_t _add_obj(ktask_obj_t *new_obj)
{
    ktask_obj_t *_obj = _head.next;
    ktask_obj_t *__obj = &_head;

    while(_obj)
    {
        if(_obj == new_obj)
            return 1;

        if(_obj->priority < new_obj->priority)
            break;

        __obj = _obj;
        _obj = _obj->next;
    }

    __obj->next = new_obj;
    new_obj->next = _obj;

    if(_period_max < new_obj->period_ms)
        _period_max = new_obj->period_ms;

    _K_PRINTF("Update Max Period: %d", _period_max);
    
    _K_PRINTF("add success");

    return 0;
}


/** 
 * @brief      ɾ�������
 * @param[in]  tar_obj ��ɾ����������ƿ�
 * @retval     0-�ɹ� -1-��������
 * @note       
 */ 
static int8_t _delete_obj(ktask_obj_t *tar_obj)
{
    ktask_obj_t *_obj = _head.next;
    ktask_obj_t *__obj = &_head;

    while(_obj)
    {
        if(_obj == tar_obj)
            break;

        __obj = _obj;
        _obj = _obj->next;
    }

    __obj->next = _obj->next;
    _obj->next = NULL;

    _update_period_max();
    
    _K_PRINTF("delete success");

    return 0;
}


