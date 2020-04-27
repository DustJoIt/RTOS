/*******************************/
/* test.cpp    */
/*******************************/
#include <cstdio>
#include "sys.h"
#include "rtos_api.h"

DeclareTask(Task1, 4);

DeclareTask(Task2, 2);

DeclareTask(Task3, 3);

DeclareTask(Task5, 2);

DeclareTask(Task6, 1);

DeclareTask(Task7, 2);

DeclareTask(Task8, 3);

DeclareTask(Task9, 11);

DeclareTask(Task10, 7);

DeclareTask(Task11, 4);

DeclareTask(Task12, 4);
DeclareSysEvent(1);
DeclareSysEvent(2);
DeclareSysEvent(3);

int main() {
/* first example: non-preemptive scheduling */
//    StartOS(Task1, Task1prior, "Task1");
//    printf("\n");

/*************************/
/* second example: semaphores */
//    InitPVS(1, "Res1");
//    InitPVS(2, "Res2");
//    StartOS(Task5, Task5prior, "Task5");
//    printf("\n");

/*************************/
/* third example: semaphores */
//    InitPVS(1, "Res1");
//    InitPVS(2, "Res2");
//    StartOS(Task9, Task9prior, "Task9");
//    printf("\n");

/*************************/
/* forth example: events */
//    StartOS(Task7, Task7prior, "Task7");
//    printf("\n");
    ShutdownOS();
    return 0;
}

/************************/
/* First example: call task 1 with task1prior=4 to show sheduler and dispathcer work */
TASK(Task1) { // priority - 4
    printf("I'm a Task_1 with priority 4 before activating task 2 with priority 2\n");
    ActivateTask(Task2, Task2prior, "Task2");
    printf("I'm a Task_1\n");
    TerminateTask();
}
TASK(Task2) { // priority - 2
    printf("I'm a Task_2 with priority 2 before activating task 3 with priority 3\n");
    ActivateTask(Task3, Task3prior, "Task3");
    printf("I'm a Task_2\n");
    TerminateTask();
}
TASK(Task3) { // priority - 3
    printf("I'm a Task_3 with priority 3\n");
    TerminateTask();
}

/************************/
/* Second example: semaphores */
TASK(Task5) { // priority - 2
    printf("Start Task5\n");
    P(1);
    ActivateTask(Task6, Task6prior, "Task6");
    V(1);
    printf("Task5\n");
    TerminateTask();
}
TASK(Task6) { // priority - 1
    printf("Start Task6\n");
    P(1);
    V(1);
    printf("Task6\n");
    TerminateTask();
}

/************************/
/* Third example: semaphores */
TASK(Task9) { // priority - 11
    printf("Start Task9\n");
    P(1);
    ActivateTask(Task10, Task10prior, "Task10");
    P(2);
    V(1);
    V(2);
    printf("Task9\n");
    TerminateTask();
}
TASK(Task10) { // priority - 7
    printf("Start Task10\n");
    P(2);
    P(1);
    V(1);
    V(2);
    printf("Task10\n");
    TerminateTask();
}

/************************/
/* Fourth example: task events*/
int Task7_ID;
TASK(Task7) { // priority - 2
    printf("Start Task7\n");
    Task7_ID = RunningTask;
    ActivateTask(Task8, Task8prior, "Task8");
    WaitEvent(Event_1);
    WaitEvent(Event_2);
    TerminateTask();
}
TASK(Task8) { // priority - 3
    printf("Start Task8\n");
    SetEvent(TaskQueue[Task7_ID], Event_1);
    SetEvent(TaskQueue[Task7_ID], Event_3);
    ClearEvent(Event_3);
    printf("Task8\n");
    TerminateTask();
    SetEvent(TaskQueue[Task7_ID], Event_2);
}