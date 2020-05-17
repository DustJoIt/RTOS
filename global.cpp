#include "sys.h"

TTask TaskQueue[MAX_TASK];
TResource ResourceQueue[MAX_RES];
size_t TaskInProcess;
size_t TaskHead;
size_t TaskCount;
TEventMask EventsInProcess;
