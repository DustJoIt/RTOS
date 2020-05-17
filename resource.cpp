#include "sys.h"
#include "rtos_api.h"
#include <cstdio>
#include <iostream>

void InitRes(size_t resNum, const std::string &name) {
    std::cout << "Resource [" << name << "] was initialized" << std::endl;
    ResourceQueue[resNum].name = name;
    ResourceQueue[resNum].task = -1;
}

void GetRes(size_t resNum) {
    std::cout << "Resource [" << ResourceQueue[resNum].name << "] was requested" << std::endl;
    while (ResourceQueue[resNum].task != -1) {
        size_t taskNum = ResourceQueue[resNum].task;
        std::cout << "Resource [" << ResourceQueue[resNum].name
                  << "] is blocked by task [" << TaskQueue[taskNum].name << "]" << std::endl;
        if (TaskQueue[taskNum].ceiling_priority >= TaskQueue[TaskInProcess].ceiling_priority) {
            std::cout << "Resource [" << ResourceQueue[resNum].name
                      << "] priority was changed to ["
                      << TaskQueue[taskNum].ceiling_priority << "]" << std::endl;
            TaskQueue[taskNum].ceiling_priority = TaskQueue[TaskInProcess].ceiling_priority;
            Schedule(taskNum);
        }
        Dispatch();
    }
    ResourceQueue[resNum].task = TaskInProcess;
}

void ReleaseRes(size_t resNum) {
    std::cout << "Resource [" << ResourceQueue[resNum].name << "] was released" << std::endl;
    size_t taskNum = ResourceQueue[resNum].task;
    ResourceQueue[resNum].task = -1;
    if (TaskQueue[taskNum].ceiling_priority != TaskQueue[taskNum].priority) {
        std::cout << "Resource [" << ResourceQueue[resNum].name
                  << "] priority was changed to ["
                  << TaskQueue[taskNum].priority << "]" << std::endl;
        TaskQueue[taskNum].ceiling_priority = TaskQueue[taskNum].priority;
    }
}