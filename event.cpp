/*********************************/
/* event.cpp */
/*********************************/
#include <cstdio>
#include "sys.h"
#include "rtos_api.h"

void SetEvent(TTask task, TEventMask mask) {
    //find task in task queue
    int task_index = -1;
    for (int i = 0; i < MAX_TASK; i++) {
        if (TaskQueue[i].entry == task.entry) {
            task_index = i;
            break;
        }
    }
    if (task_index == -1) {
        printf("Task was not found\n");
        return;
    }

    //get index of first free place in working events
    TEventMask* working_events = TaskQueue[task_index].working_events;
    int nextWorkingEvent = 0;
    while (nextWorkingEvent < MAX_EVENT) {
        if (working_events[nextWorkingEvent] != 0) {
            printf("Working Event (set) %i for task %s\n", working_events[nextWorkingEvent], TaskQueue[RunningTask].name);
            nextWorkingEvent++;
        } else {
            break;
        }
    }

    working_events[nextWorkingEvent] = mask;
    printf("Set Working Event %i for task %s\n", working_events[nextWorkingEvent], TaskQueue[task_index].name);
    TEventMask* waiting_events = TaskQueue[task_index].waiting_events;
    //if task is waiting & newly set mask == waiting mask (one at least) -> task is ready
    if (TaskQueue[task_index].task_state == TASK_WAITING) {
        for (int i = 0; i < MAX_EVENT; i++) {
            if (mask == waiting_events[i]) {
                waiting_events[i] = 0;
                ArrayOffset(i, waiting_events);
                TaskQueue[task_index].task_state = TASK_READY;
                printf("Task \"%s\" is ready\n", TaskQueue[task_index].name);
                break;
            }
        }
    }
    Dispatch();
    printf("End of SetEvent %i for task %s\n", mask, TaskQueue[task_index].name);
}

void ArrayOffset(int index, TEventMask* events) {
    for (int i = index; i < MAX_EVENT - 1; i++) {
        events[i] = events[i + 1];
    }
    events[MAX_EVENT - 1] = 0;
}

void GetEvent(TTask task, TEventMask *mask) {
    *mask = *task.working_events;
}

void ClearEvent(TEventMask mask) {
    printf("In clear function with mask %i\n", mask);
    for (int i = 0; i < MAX_TASK; i++) {
        TEventMask* working_task = TaskQueue[i].working_events;
        for (int j = 0; j < MAX_EVENT; j++) {
            if (working_task[j] == mask) {
                working_task[j] = 0;
                ArrayOffset(j, working_task);
                printf("Clear event %i from task %s\n", mask, TaskQueue[i].name);
            }
        }
    }
}

void WaitEvent(TEventMask mask) {
    printf("Working task - %s\n", TaskQueue[RunningTask].name);
    TEventMask* waiting_events = TaskQueue[RunningTask].waiting_events;
    int nextWaitingEvent = 0;
    //find first free place in waiting events
    while (nextWaitingEvent < MAX_EVENT) {
        if (waiting_events[nextWaitingEvent] != 0) {
            printf("Waiting Event (wait) %i for task %s\n", waiting_events[nextWaitingEvent], TaskQueue[RunningTask].name);
            nextWaitingEvent++;
        } else {
            break;
        }
    }

    //set new waiting mask for running task
    int run_task = RunningTask;
    printf("Set WaitEvent %i for task %s\n", mask, TaskQueue[RunningTask].name);
    waiting_events[nextWaitingEvent] = mask;
    TEventMask* working_events = TaskQueue[RunningTask].working_events;
    //if new waiting mask != any working events -> task is waiting
    for (int i = 0; i < MAX_EVENT; i++) {
        if ((working_events[i] & mask) == 0) {
            TaskQueue[RunningTask].task_state = TASK_WAITING;
            Dispatch();
            break;
        }
    }

    for (int i = 0; i < MAX_EVENT; i++) {
        if (working_events[i] == mask) {
            working_events[i] = 0;
            ArrayOffset(i, working_events);
            printf("Delete working event %i from task %s\n", mask, TaskQueue[run_task].name);
            break;
        }
    }
    printf("End of WaitEvent %i for task %s\n", mask, TaskQueue[run_task].name);
}
