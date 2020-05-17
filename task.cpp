#include <cstdio>
#include <string>
#include "sys.h"
#include "rtos_api.h"

bool checkIfNextExists(size_t taskNum) {
    return TaskQueue[taskNum].next != -1;
}

void InsertBefore(size_t existingTaskNum, size_t insertTaskNum, size_t *head) {
    if (insertTaskNum == *head) {
        *head = existingTaskNum;
    }
    TaskQueue[existingTaskNum].next = insertTaskNum;
    TaskQueue[existingTaskNum].prev = TaskQueue[insertTaskNum].prev;
    TaskQueue[TaskQueue[insertTaskNum].prev].next = existingTaskNum;
    TaskQueue[insertTaskNum].prev = existingTaskNum;
}

int FindFreeTask() {
    for (int i = 0; i < MAX_TASK; i++) {
        if (TaskQueue[i].task_state == TTaskState::TASK_SUSPENDED) {
            return i;
        }
    }

    return -1;
}

void InsertAfter(size_t existingTaskNum, size_t insertTaskNum) {
    TaskQueue[existingTaskNum].next = TaskQueue[insertTaskNum].next;
    TaskQueue[existingTaskNum].prev = insertTaskNum;
    if (TaskQueue[insertTaskNum].next != -1) {
        TaskQueue[TaskQueue[insertTaskNum].next].prev = existingTaskNum;
    }
    TaskQueue[insertTaskNum].next = existingTaskNum;
}

void RemoveTask(size_t existingTaskNum, size_t *head) {
    if (*head == existingTaskNum) {
        *head = TaskQueue[existingTaskNum].next;
    }

    if (TaskQueue[existingTaskNum].prev != -1) {
        TaskQueue[TaskQueue[existingTaskNum].prev].next = TaskQueue[existingTaskNum].next;
    } 
    if (TaskQueue[existingTaskNum].next != -1) {
        TaskQueue[TaskQueue[existingTaskNum].next].prev = TaskQueue[existingTaskNum].prev;
    }
}

void ActivateTask(TTaskCall entry, size_t priority, const std::string &name) {
    size_t occupy;
    std::cout << "Task [" << name << "] activation" << std::endl;
    occupy = FindFreeTask();

    TaskQueue[occupy].priority = priority;
    TaskQueue[occupy].ceiling_priority = priority;
    TaskQueue[occupy].name = name;
    TaskQueue[occupy].entry = entry;
    TaskQueue[occupy].switch_count = 0;
    TaskQueue[occupy].task_state = TTaskState::TASK_READY;
    TaskCount++;

    Schedule(occupy);
}

void TerminateTask() {
    size_t task;
    TaskCount--;
    task = TaskInProcess;

    std::cout << "Task [" << TaskQueue[task].name << "] termination started" << std::endl;
    TaskQueue[task].task_state = TTaskState::TASK_SUSPENDED;
    RemoveTask(task, &TaskHead);

    // if (TaskCount == 0) {
    //     longjmp(MainContext, 1);
    // }
    Dispatch();
}

void Schedule(size_t task) {
    size_t cur, priority;
    if (task <= sizeof(TaskQueue) / sizeof(TaskQueue[0])
        && TaskQueue[task].task_state == TTaskState::TASK_SUSPENDED) {
        return;
    }

    std::cout << "Started scheduling of the [" << TaskQueue[task].name << "] task" << std::endl;
    if (TaskHead == -1) {
        TaskHead = task;
    } else if (TaskCount > 1) {
        cur = TaskHead;
        if (cur == task) {
            cur = TaskQueue[cur].next;
        }
        priority = TaskQueue[task].ceiling_priority;
        RemoveTask(task, &TaskHead);


        while (TaskQueue[cur].ceiling_priority >= priority) {
            if (TaskQueue[cur].next == -1) {
                break;
            }
            cur = TaskQueue[cur].next;
        }

        if (TaskQueue[cur].ceiling_priority >= priority || cur == TaskHead) {
            InsertAfter(task, cur);
        } else {
            InsertBefore(task, cur, &TaskHead);
        }
    }
}

void TaskSwitch(size_t nextTask) {
    if (nextTask == -1) {
        return;
    }

    TaskQueue[nextTask].task_state = TTaskState::TASK_RUNNING;
    TaskInProcess = nextTask;
    TaskQueue[nextTask].switch_count++;

    if (TaskQueue[nextTask].switch_count == 1) {
        TaskQueue[nextTask].entry();
    } else {
        longjmp(TaskQueue[nextTask].context, 1); // the task context already exists
    }
}

// reserving the stack area for the task
void TaskSwitchWithCushion(int nextTask) {
    char space[100000];
    space[99999] = 1;
    TaskSwitch(nextTask);
}

void Dispatch() {
    if (TaskInProcess == -1) {
        // Должны попытаться переключиться на TaskHead
        if (TaskHead == -1) {
            return;
        }

        if (TaskQueue[TaskHead].task_state == TTaskState::TASK_READY) {
            TaskSwitch(TaskHead);
        }
        return;
    }
    if (TaskInProcess != -1 && TaskQueue[TaskInProcess].task_state == TTaskState::TASK_RUNNING) {
        TaskQueue[TaskInProcess].task_state = TTaskState::TASK_READY;
    }
    size_t nextTask = TaskQueue[TaskInProcess].next;
    while (TaskCount) {
        if (TaskQueue[nextTask].task_state == TTaskState::TASK_READY) {
            // std::cout << "Ended the dispatch of the task [" << TaskQueue[TaskInProcess].name << "]" << std::endl;
            // if (TaskQueue[TaskInProcess].task_state == TTaskState::TASK_SUSPENDED) {
            if (TaskQueue[TaskInProcess].task_state != TTaskState::TASK_SUSPENDED) {
                Schedule(TaskInProcess);
            }

            if (TaskInProcess == -1 || TaskQueue[TaskInProcess].task_state == TTaskState::TASK_SUSPENDED)
                TaskSwitch(nextTask);
            else {
                if (!setjmp(TaskQueue[TaskInProcess].context)) {
                    if (TaskQueue[TaskInProcess].switch_count == 1)
                        TaskSwitchWithCushion(nextTask);
                    else
                        TaskSwitch(nextTask);
                }
            }
            // }
            break;
        }
        nextTask = TaskQueue[nextTask].next;
        if (nextTask == -1) {
            std::cout << "No ready tasks at the moment" << std::endl;
            break;
        }
    }
}