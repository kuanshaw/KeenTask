#include "k_rtos.h"

/** 
 * 滴答计数值   
 */ 
static uint16_t _tick = 0;

/** 
 * 最大任务执行周期
 */ 
static uint16_t _period_max = 0;

/** 
 * 任务链头结点
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
 * @brief      开始任务
 * @param[in]  obj 任务控制块
 * @retval     0-成功 -1-参数错误
 * @note       
 */ 
int8_t ktask_start(ktask_obj_t *obj)
{
    return _add_obj(obj);
}


/** 
 * @brief      删除任务
 * @param[in]  obj 任务控制块
 * @retval     0-成功 -1-参数错误 -2-任务未初始化
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
 * @brief      挂起任务
 * @param[in]  obj 任务控制块
 * @retval     0-成功 -1-参数错误 -2-任务未初始化
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
 * @brief      恢复任务
 * @param[in]  obj 任务控制块
 * @retval     0-成功 -1-参数错误 -2-任务未初始化
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
 * @brief      打印当前任务
 * @param      无
 * @retval     无
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
 * @brief      任务调度函数
 * @param      无
 * @retval     无
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
 * @brief      任务滴答函数
 * @param      无
 * @retval     无
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
 * @brief      任务错误
 * @param      无
 * @retval     无
 * @note       
 */ 
void ktask_assert(void)
{
    _K_PRINTF("assert!!!");
    while(1);
}


/** 
 * @brief      任务控制块初始化
 * @param[in]  obj 任务控制块
 * @retval     无
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
 * @brief      更新当前任务链中最大的周期
 * @param      无
 * @retval     无
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
 * @brief      插入任务块
 * @param[in]  new_obj 待插入的任务控制块
 * @retval     0-成功 -1-参数错误
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
 * @brief      删除任务块
 * @param[in]  tar_obj 待删除的任务控制块
 * @retval     0-成功 -1-参数错误
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


