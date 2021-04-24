#include "ktask.h"

#define _KTASK_TRY(obj) \
{ \
    if(obj == NULL) { \
        _K_PRINTF("error, obj is null, please check your code"); \
        return -1; \
    } \
    if(!(obj->flag & KTASK_FLAG_OPEN)) { \
        _K_PRINTF("error, task isn't create, please use ktask_create() to create"); \
        return -2; \
    } \
}

/** 
 * 滴答计数值，调用一次ktask_tick()加一
 */ 
static uint16_t _ktick = 0;

/** 
 * 最大任务执行周期，单位ms
 */ 
static uint16_t _period_ms_max = 0;

/** 
 * 任务链表头指针
 */ 
static ktask_obj_t *_head = NULL;

static void _reset_obj(ktask_obj_t *obj);
static void _update_period_ms_max(void);
static int8_t _add_obj(ktask_obj_t *new_obj);
static int8_t _delete_obj(ktask_obj_t *tar_obj);


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
ktask_obj_t * ktask_create(ktask_obj_t *obj, const char *name, ktask_ptr new_task, uint32_t period_ms, uint8_t priority, void *arg)
{
    //TODO: Check param
    if((obj == NULL) || (name == NULL) || (new_task == NULL))
        return NULL; 

    //TOOD: reset
    _reset_obj(obj);

    //TODO: set value
    _K_MEMCPY(obj->name, name, KTASK_NAME_MAX);
    obj->period_ms = period_ms;
    obj->flag |= KTASK_FLAG_OPEN;   //set open
    obj->priority = priority;
    obj->task = new_task;
    obj->arg = arg;
    
    _K_PRINTF("create [%s] successful", obj->name);
    return obj;
}


/** 
 * @brief      开始任务
 * @param[in]  obj 任务对象
 * @retval     0-成功
 * @note       
 */ 
int8_t ktask_start(ktask_obj_t *obj)
{
    _KTASK_TRY(obj);

    if(_add_obj(obj))
    {
        _K_PRINTF("start [%s] fail", obj->name);
        return -3;
    }

    if(obj->period_ms > _period_ms_max)
        _period_ms_max = obj->period_ms;

    _K_PRINTF("start [%s] successful", obj->name);
    return 0;
}


/** 
 * @brief      删除任务
 * @param[in]  obj 任务对象
 * @retval     0-成功
 * @note       
 */ 
int8_t ktask_delete(ktask_obj_t *obj)
{
    _KTASK_TRY(obj);
    
    if(_delete_obj(obj))
    {
        _K_PRINTF("delete [%s] fail", obj->name);
        return -3;
    }

    obj->flag = KTASK_FLAG_CLOSE;   //set close, clear all flag

    if(obj->period_ms >= _period_ms_max)
        _update_period_ms_max();

    _K_PRINTF("delete [%s] successful", obj->name);
    return 0;
}


/** 
 * @brief      挂起任务
 * @param[in]  obj 任务对象
 * @retval     0-成功
 * @note       
 */ 
int8_t ktask_suspend(ktask_obj_t *obj)
{
    _KTASK_TRY(obj);

    obj->flag |= KTASK_FLAG_SUSPEND;    //set suppend
    
    return 0;
}


/** 
 * @brief      恢复任务
 * @param[in]  obj 任务对象
 * @retval     0-成功
 * @note       
 */ 
int8_t ktask_resume(ktask_obj_t *obj)
{
    _KTASK_TRY(obj);

    obj->flag &= (~KTASK_FLAG_SUSPEND); //clear suppend

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
    ktask_obj_t *_obj = _head;

    while(_obj)
    {
        _K_PRINTF("> Name: %s, Period: %d, Priority: %d, Flag: 0x%02X", _obj->name, \
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
    ktask_obj_t *_obj = _head;

    while(_obj)
    {
        if(_obj->flag & KTASK_FLAG_READY)
        {
            _obj->task(_obj->arg);
            _obj->flag &= (~KTASK_FLAG_READY);
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
    ktask_obj_t *_obj = _head;
    uint16_t _tick_max = _period_ms_max / period;
    uint16_t _obj_tick = 0;

    //TODO: _ktick increase
    _ktick++;

    while(_obj)
    {
        //TODO: calculate current task tick
        _obj_tick = _obj->period_ms / period;

        //TODO: set ready flag
        if(((_ktick % _obj_tick) == 0) && (!(_obj->flag & KTASK_FLAG_SUSPEND)))
            _obj->flag |= KTASK_FLAG_READY;

        _obj = _obj->next;
    }

    //TODO: _ktick clear
    if(_ktick >= _tick_max)
        _ktick = 0;
}


/** 
 * @brief      任务对象初始化
 * @param[in]  obj 任务对象
 * @retval     无
 * @note       
 */ 
static void _reset_obj(ktask_obj_t *obj)
{
    _K_MEMSET(obj->name, '\0', KTASK_NAME_MAX);
    obj->tick = 0;
    obj->flag = KTASK_FLAG_CLOSE;
    obj->priority = 0;
    obj->task = NULL;
    obj->next = NULL;   //important!
}


/** 
 * @brief      更新当前任务链中最大的周期
 * @param      无
 * @retval     无
 * @note       
 */ 
static void _update_period_ms_max(void)
{
    ktask_obj_t *_obj = _head;

    _period_ms_max = 0;
    while(_obj)
    {
        if((_period_ms_max < _obj->period_ms) && (!(_obj->flag & KTASK_FLAG_SUSPEND)))
            _period_ms_max = _obj->period_ms;

        _obj = _obj->next;
    }

    _K_PRINTF("Update Max Period: %dms", _period_ms_max);
}


/** 
 * @brief      插入任务块
 * @param[in]  new_obj 待插入的任务对象
 * @retval     0-成功
 * @note       
 */ 
static int8_t _add_obj(ktask_obj_t *new_obj)
{
    ktask_obj_t *_obj = _head;
    ktask_obj_t *__obj = NULL;

    while(_obj)
    {
        if(_obj == new_obj)
            return -1;

        if(_obj->priority > new_obj->priority)
            break;

        __obj = _obj;
        _obj = _obj->next;
    }

    if(_obj == NULL)
    {
        if(__obj != NULL)
        {
            // _K_PRINTF("insert in the end");
            __obj->next = new_obj;
        }
        else
        {
            // _K_PRINTF("list is none");
            _head = new_obj;
        }
    }
    else
    {
        if(__obj != NULL)
        {
            // _K_PRINTF("insert middle");
            __obj->next = new_obj;
            new_obj->next = _obj;
        }
        else
        {
            // _K_PRINTF("insert in the head");
            _head = new_obj;
            new_obj->next = _obj;
        }
    }

    return 0;
}


/** 
 * @brief      删除任务块
 * @param[in]  tar_obj 待删除的任务对象
 * @retval     0-成功
 * @note       
 */ 
static int8_t _delete_obj(ktask_obj_t *tar_obj)
{
    ktask_obj_t *_obj = _head;
    ktask_obj_t *__obj = NULL;

    while(_obj)
    {
        if(_obj == tar_obj)
            break;

        __obj = _obj;
        _obj = _obj->next;
    }

    if(_obj == NULL)
    {
        // _K_PRINTF("delete no found");
        return -1;
    }

    if(__obj != NULL)
    {
        // _K_PRINTF("delete middle or end");
        __obj->next = _obj->next;
        _obj->next = NULL;
    }
    else
    {
        // _K_PRINTF("delete in head");
        _head = _obj->next;
        _obj->next = NULL;
    }

    return 0;
}
