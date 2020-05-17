#include <cstdio>
#include "sys.h"
#include "rtos_api.h"

void InitializeTask(TTask &task, size_t next, size_t prev) {
    task.next = -1;
    task.prev = -1;
    task.task_state = TTaskState::TASK_SUSPENDED;
    task.switch_count = 0;
    task.waiting_events = 0;
}

void StartOS(TTaskCall &task, size_t priority, const std::string &name) {
    size_t i;
    TaskInProcess = TaskHead = -1;
    TaskCount = EventsInProcess = 0;

    std::cout << "OS initialized..." << std::endl;
    for (i = 0; i < MAX_TASK; i++) {
        InitializeTask(TaskQueue[i], i + 1, i - 1);
    }
    TaskQueue[MAX_TASK - 1].next = 0;
    TaskQueue[0].prev = MAX_TASK - 1;

    ActivateTask(task, priority, name);
    Dispatch();
}

void ShutdownOS() {
    std::cout << "OS shutdown..." << std::endl;
}