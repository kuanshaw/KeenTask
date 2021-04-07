#include "k_rtos.h"



static uint16_t _tick = 0;
static uint16_t _period_max = 0;

static ktask_obj_t _head = 
{
    .name = "head",
    .next = NULL,
};


static void _reset_obj(ktask_obj_t *obj)
{
    _K_MEMSET(obj->name, '\0', KTASK_NAME_MAX);
    obj->tick = 0;
    obj->flag = 0;
    obj->priority = 0;
    obj->task = NULL;
}


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


int8_t ktask_create(ktask_obj_t *obj, const char *name, ktask_ptr new_task, uint32_t period_ms, uint8_t priority, void *arg)
{
    if((obj == NULL) || (name == NULL) || (new_task == NULL))
        return -2; 

    _reset_obj(obj);

    _K_MEMCPY(obj->name, name, KTASK_NAME_MAX);
    obj->period_ms = period_ms;
    obj->flag |= KTASK_FLAG_OPEN;
    obj->priority = priority;
    obj->task = new_task;
    obj->arg = arg;

    return _add_obj(obj);
}

int8_t ktask_delete(ktask_obj_t *obj)
{
    if(obj == NULL)
        return -1; 
        
    if(!(obj->flag & KTASK_FLAG_OPEN))
        return -2;

    obj->flag = KTASK_FLAG_CLOSE;

    return _delete_obj(obj);
}


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

int8_t ktask_resume(ktask_obj_t *obj)
{
    if(obj == NULL)
        return -1; 
        
    if(!(obj->flag & KTASK_FLAG_OPEN))
        return -2;

    obj->flag &= (~KTASK_FLAG_SUSPEND);

    return 0;
}


void ktask_list(void)
{
    ktask_obj_t *_obj = _head.next;

    while(_obj)
    {
        _K_PRINTF("name: %s, pri: %d", _obj->name, _obj->priority);

        _obj = _obj->next;
    }
}


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


void ktask_assert(void)
{
    _K_PRINTF("assert!!!");
    while(1);
}






#include "unistd.h"

int time = 0;
void test(void *arg)
{
    // if(*(char *)arg == '3')
    // {
    //     time = 2;
    //     printf("change time~~~\r\n");
    // }

    printf("TICK: %d | task %s\r\n", _tick, (char *)arg);
}

int main(int argc, char const *argv[])
{
    ktask_obj_t obj[5];
    static char str_task[][KTASK_NAME_MAX] = {
        "1",
        "2",
        "3",
        "4",
        "5",
    };

    ktask_create(&obj[0], "task1", test, 10, 3, str_task[0]);
    ktask_create(&obj[1], "task2", test, 2, 6, str_task[1]);
    ktask_create(&obj[2], "task3", test, 4, 2, str_task[2]);
    ktask_create(&obj[3], "task4", test, 2, 5, str_task[3]);
    ktask_create(&obj[4], "task5", test, 8, 9, str_task[4]);

    ktask_list();

    ktask_delete(&obj[0]);

    ktask_suspend(&obj[4]);

    ktask_list();

    time = 1;

    while(1)
    {
        ktask_tick(time);
        ktask_schedule();
        sleep(time);
    }

    return 0;
}







