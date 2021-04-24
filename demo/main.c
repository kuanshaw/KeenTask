
#include "ktask.h"



void task(void *arg)
{
    printf("%s, %s\r\n", (char *)arg, __TIME__);
}

int main(int argc, char const *argv[])
{
    /* code */
    ktask_obj_t A;
    ktask_obj_t B;
    ktask_obj_t C;
    ktask_obj_t D;

    ktask_create(&A, "A", task, 1000, 3, "A"); ktask_start(&A);
    ktask_create(&B, "B", task, 1000, 1, "B"); ktask_start(&B);
    ktask_create(&C, "C", task, 3000, 4, "C"); ktask_start(&C);
    ktask_create(&D, "D", task, 6000, 2, "D"); ktask_start(&D);

    while(1)
    {
        ktask_tick(1000);
        ktask_schedule();
        printf("===========\r\n");
        sleep(1);
    }

    

    return 0;
}
