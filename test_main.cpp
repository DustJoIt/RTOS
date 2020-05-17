#include <cstdio>
#include <cstdlib>
#include <clocale>
#include "sys.h"
#include "rtos_api.h"

DeclareTask(Task1, 4);

DeclareTask(Task2, 2);

DeclareTask(Task3, 1);
DeclareTask(Task4, 4);

DeclareTask(Task5, 2);

DeclareTask(Task6, 1);

DeclareTask(Task7, 2);

DeclareTask(Task8, 3);

DeclareTask(Task9, 11);

DeclareTask(Task10, 7);
DeclareSysEvent(1);
DeclareSysEvent(2);
DeclareSysEvent(3);

int main() {
    setlocale(LC_CTYPE, "");
    InitRes(1, "Res1");
    InitRes(2, "Res2");
    StartOS(Task1, Task1prior, "Task1");
    ShutdownOS();
    return 0;
}
//4
TASK(Task1) {
    std::cout << "Start Task1" << std::endl;
    ActivateTask(Task2, Task2prior, "Task2");
    std::cout << "Task1" << std::endl;
    TerminateTask();
}
//2
TASK(Task2) {
    std::cout << "Start Task2" << std::endl;
    ActivateTask(Task3, Task3prior, "Task3");
    Dispatch();
    Dispatch();

    std::cout << "Task2" << std::endl;
    TerminateTask();
}

TASK(Task3) {
    std::cout << "Start Task3" << std::endl;
    Dispatch();
    std::cout << "Task3" << std::endl;
    TerminateTask();
}

TASK(Task4) {
    std::cout << "Start Task4" << std::endl;
    std::cout << "Task4" << std::endl;
    TerminateTask();
}
//-------------------------------------
// Resource Managment Example
//
TASK(Task5) {
    printf("Start Task5\n");
    GetRes(1);
    ActivateTask(Task6, Task6prior, "Task6");
    Dispatch();
    ReleaseRes(1);
    printf("Task5\n");
    TerminateTask();
}
TASK(Task6) {
    printf("Start Task6\n");
    GetRes(1);
    ReleaseRes(1);
    printf("Task6\n");
    TerminateTask();
}
//-------------------------------------
// Deadlock
//
TASK(Task9) {
    std::cout << "Start Task9" << std::endl;
    GetRes(1);
    ActivateTask(Task10, Task10prior, "Task10");
    Dispatch();
    GetRes(2);
    ReleaseRes(1);
    ReleaseRes(2);
    std::cout << "Task9" << std::endl;
    TerminateTask();
}

TASK(Task10) {
    std::cout << "Start Task10" << std::endl;
    GetRes(2);
    GetRes(1);
    ReleaseRes(1);
    ReleaseRes(2);
    std::cout << "Task10" << std::endl;
    TerminateTask();
}

//-------------------------------------
// Events
//
size_t Task7_ID;

TASK(Task7) {
    std::cout << "Start Task7" << std::endl;
    Task7_ID = TaskInProcess;
    ActivateTask(Task8, Task8prior, "Task8");
    GetRes(1);
    WaitSysEvent(Event_1);
    ReleaseRes(1);
    TEventMask evnt;
    GetSysEvent(&evnt);
    std::cout << "GetEvent= " << evnt << std::endl;
    std::cout << "Task7" << std::endl;
    TerminateTask();
}
//3
TASK(Task8) {
    std::cout << "Start Task8" << std::endl;
    SetSysEvent(Event_1);
    GetRes(1);
    ReleaseRes(1);
    std::cout << "Task8" << std::endl;
    TerminateTask();
}
