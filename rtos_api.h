#pragma once

#include "sys.h"

#define DeclareTask(TaskID, priority)\
TASK(TaskID); \
enum {TaskID##prior=priority}
#define TASK(TaskID) void TaskID(void)
#define DeclareSysEvent(ID) \
const size_t Event_##ID = (ID)*(ID);

typedef void TTaskCall();

void ActivateTask(TTaskCall entry, size_t priority, const std::string &name);

void TerminateTask();

void StartOS(TTaskCall &task, size_t priority, const std::string &name);

void ShutdownOS();

void InitRes(size_t resNum, const std::string &name);

void GetRes(size_t resNum);

void ReleaseRes(size_t resNum);

void SetSysEvent(const TEventMask &eventMask);

void GetSysEvent(TEventMask *eventMask);

void WaitSysEvent(const TEventMask &eventMask);