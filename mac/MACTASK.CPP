#include "DDAccess.h"
#include "TaskSystem.h"
#include "pserror.h"
osMutex::osMutex() {}
osMutex::~osMutex() { Destroy(); }
bool osMutex::Create() { return false; }
void osMutex::Destroy() {}
bool osMutex::Acquire(int timeout) {}
void osMutex::Release() {}