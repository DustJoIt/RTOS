#include <cstdio>

#include "sys.h"
#include "rtos_api.h"

void SetSysEvent(const TEventMask &eventMask) {
    std::cout << "Setting the system event: [" << eventMask << "]" << std::endl;
    EventsInProcess |= eventMask;
    for (auto &task : TaskQueue) {
        if (task.task_state == TTaskState::TASK_WAITING
            && (EventsInProcess & task.waiting_events)) {
            task.waiting_events &= !eventMask;
            task.task_state = TTaskState::TASK_READY;
            std::cout << "Task [" << task.name << "] status set to READY" << std::endl;
        }
    }
    EventsInProcess &= !eventMask;
    std::cout << "End of setting of the system event: [" << eventMask << "]" << std::endl;
}

void GetSysEvent(TEventMask *eventMask) {
    *eventMask = EventsInProcess;
}

void WaitSysEvent(const TEventMask &eventMask) {
    std::cout << "Waiting for system event: [" << eventMask << "]" << std::endl;
    TaskQueue[TaskInProcess].waiting_events = eventMask;
    if ((EventsInProcess & eventMask) == 0) {
        TaskQueue[TaskInProcess].task_state = TTaskState::TASK_WAITING;
        Dispatch();
    }
    std::cout << "End of waiting for system event: [" << eventMask << "]" << std::endl;
}
