#pragma once

#include "defs.h"
#include <csetjmp>
#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>

enum class TTaskState {
    TASK_RUNNING, 
    TASK_READY,
    TASK_SUSPENDED,
    TASK_WAITING
};

typedef size_t TEventMask;

typedef struct {
    size_t next, prev;
    size_t priority;
    size_t ceiling_priority;

    void (*entry)();

    std::string name;
    TTaskState task_state;
    size_t switch_count;
    jmp_buf context;
    TEventMask waiting_events;
} TTask;

typedef struct {
    size_t task;
    std::string name;
} TResource;

extern TTask TaskQueue[MAX_TASK];
extern TResource ResourceQueue[MAX_RES];
extern size_t TaskInProcess;
extern size_t TaskHead;
extern size_t TaskCount;
extern TEventMask EventsInProcess;

void Schedule(size_t task);

void Dispatch();