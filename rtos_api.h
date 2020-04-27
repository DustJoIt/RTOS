/****************************************/
/* rtos_api.h */
/****************************************/
#pragma once

#include "sys.h"
#include <string>

#define DeclareTask(TaskID, priority)\
TASK(TaskID); \
enum {TaskID##prior=(priority)}
#define TASK(TaskID) void TaskID(void)

typedef void TTaskCall(void);

int ActivateTask(TTaskCall entry, int priority, char *name);

void TerminateTask(void);

int StartOS(TTaskCall entry, int priority, char *name);

void ShutdownOS();

void InitPVS(TSemaphore S, std::string name);

void P(TSemaphore S);

void V(TSemaphore S);

#define DeclareSysEvent(ID) \
const int Event_##ID = (ID);

void SetEvent(TTask task, TEventMask mask);

void GetEvent(TTask task, TEventMask *mask);

void WaitEvent(TEventMask mask);

void ClearEvent(TEventMask mask);

void ArrayOffset(int index, TEventMask* events);
