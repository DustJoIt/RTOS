/*********************************/
/* task.cpp */
/*********************************/
#include <stdio.h>
#include "sys.h"
#include "rtos_api.h"

// to add task before item, head indicates the head of the queue
void InsertTaskBefore(int task, int item, int *head) {
    if (TaskQueue[task].next != -1)
        return;
    if (item == *head)
        *head = task;
    TaskQueue[task].next = item;
    TaskQueue[task].prev = TaskQueue[item].prev;
    TaskQueue[TaskQueue[item].prev].next = task;
    TaskQueue[item].prev = task;
}

void InsertTaskAfter(int task, int item) {
    if (TaskQueue[task].next != -1)
        return;
    TaskQueue[task].next = TaskQueue[item].next;
    TaskQueue[task].prev = item;
    TaskQueue[TaskQueue[item].next].prev = task;
    TaskQueue[item].next = task;
}

void RemoveTask(int task, int *head) {
    if (TaskQueue[task].next == -1)
        return;
    if (*head == task) {
        if (TaskQueue[task].next == task)
            *head = -1;
        else
            *head = TaskQueue[task].next;
    }
    TaskQueue[TaskQueue[task].prev].next = TaskQueue[task].next;
    TaskQueue[TaskQueue[task].next].prev = TaskQueue[task].prev;
    TaskQueue[task].next = -1;
    TaskQueue[task].prev = -1;
}

int ActivateTask(TTaskCall entry, int priority, char *name) {
    int occupy;
    printf("Start activating task %s\n", name);
    occupy = FreeTask;
    // change the queue of free tasks
    RemoveTask(occupy, &FreeTask);
    TaskQueue[occupy].priority = priority;
    TaskQueue[occupy].ceiling_priority = priority;
    TaskQueue[occupy].name = name;
    TaskQueue[occupy].entry = entry;
    TaskQueue[occupy].switch_count = 0;
    TaskQueue[occupy].task_state = TASK_READY;
    TaskCount++;
    Schedule(occupy);
    if (TaskCount == 1) {
        Dispatch();
    }
    return occupy;
}

void TerminateTask(void) {
    int task;
    TaskCount--;
    task = RunningTask;
    printf("TerminateTask %s\n", TaskQueue[task].name);
    TaskQueue[task].task_state = TASK_SUSPENDED;
    RemoveTask(task, &(HeadTasks[TaskQueue[task].ceiling_priority]));
    // add task to the queue of free tasks
    InsertTaskBefore(task, FreeTask, &FreeTask);
    // if no task left -> return
    if (TaskCount == 0)
        longjmp(MainContext, 1);
    Dispatch();
}

void Schedule(int task) {
    int priority;
    if (TaskQueue[task].task_state == TASK_SUSPENDED)
        return;
    printf("Start scheduling %s\n", TaskQueue[task].name);
    priority = TaskQueue[task].ceiling_priority;
    RemoveTask(task, &(HeadTasks[priority]));
    if (HeadTasks[priority] == -1) {
        HeadTasks[priority] = task;
        TaskQueue[task].next = task;
        TaskQueue[task].prev = task;
    } else
        InsertTaskAfter(task, TaskQueue[HeadTasks[priority]].prev);
}

void TaskSwitch(int nextTask) {
    if (nextTask == -1)
        return;
    TaskQueue[nextTask].task_state = TASK_RUNNING;
    RunningTask = nextTask;
    TaskQueue[nextTask].switch_count++;
    if (TaskQueue[nextTask].switch_count == 1)
        TaskQueue[nextTask].entry(); // if for the first time
    else
        longjmp(TaskQueue[nextTask].context, 1); // the task context already exists
}

// reserving the stack area for the task
void TaskSwitchWithCushion(int nextTask) {
    char space[100000];
    space[99999] = 1;
    TaskSwitch(nextTask);
}

void Dispatch() {
    if (RunningTask != -1)
        printf("Dispatch - %s\n", TaskQueue[RunningTask].name);
    else
        printf("Dispatch\n");
    int nextTask = HeadTasks[0];
    int priority = 0;
    while (TaskCount) {
        if (nextTask != -1) {
            if (TaskCount == 1 && TaskQueue[nextTask].task_state != TASK_READY)
                break;
            if (TaskQueue[nextTask].task_state == TASK_READY || TaskQueue[nextTask].task_state == TASK_RUNNING) {
                // switch to the next task
                if (RunningTask == -1 || TaskQueue[RunningTask].task_state == TASK_SUSPENDED)
                    TaskSwitch(nextTask);
                else if (RunningTask != nextTask)
                    if (!setjmp(TaskQueue[RunningTask].context)) {
                        if (TaskQueue[RunningTask].switch_count == 1)
                            TaskSwitchWithCushion(nextTask);
                        else
                            TaskSwitch(nextTask);
                    }
                break;
            }
            nextTask = TaskQueue[nextTask].next;
        }
        if (nextTask == -1 || nextTask == HeadTasks[priority]) { // switch to the next priority
            priority++;
            if (priority < MAX_PRIORITY)
                nextTask = HeadTasks[priority];
            else {
                longjmp(MainContext, 1);
            }
        }
    }
    printf("End of Dispatch - %s\n", TaskQueue[RunningTask].name);
}