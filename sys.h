/****************************************/
/* sys.h /
/****************************************/
#pragma once

#include "defs.h"
#include <csetjmp>
#include <string>

enum TTaskState {
    TASK_RUNNING,
    TASK_READY,
    TASK_SUSPENDED,
    TASK_WAITING
};

typedef unsigned int TEventMask;

typedef int TSemaphore;

typedef struct Type_Task {
    int next, prev;
    int priority;
    int ceiling_priority;

    void (*entry)(void);

    char *name;
    TTaskState task_state;
    int switch_count;
    jmp_buf context;
    TSemaphore waited_resource;

    TEventMask waiting_events[MAX_EVENT];
    TEventMask working_events[MAX_EVENT];
} TTask;

typedef struct Type_resource {
    volatile int block;
    std::string name;
} TResource;

extern TTask TaskQueue[MAX_TASK]; // queue with tasks to execute
extern TResource ResourceQueue[MAX_RES];
extern int RunningTask; // running task
extern int HeadTasks[MAX_PRIORITY]; // first task in the queue
extern int TaskCount; // number of tasks left to complete
extern int FreeTask; // pointer to the first available task (the rest in the queue after it)
extern jmp_buf MainContext; // main context
void Schedule(int task);
void Dispatch();