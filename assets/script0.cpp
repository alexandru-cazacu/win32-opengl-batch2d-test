#include "hyper_engine.h"

HYPER_SCRIPT void OnUpdate(void)
{
    MessageBoxA(NULL, "Hello from script0.dll", "script0.dll", MB_ICONWARNING | MB_CANCELTRYCONTINUE);
}
