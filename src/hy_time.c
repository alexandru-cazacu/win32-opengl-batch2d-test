global_variable double g_TimerStart;
global_variable double g_TimerFrequency = 0.0f;

internal void hy_timer_init()
{
    if (g_TimerFrequency == 0.0f) {
        QueryPerformanceFrequency((LARGE_INTEGER*)&g_TimerFrequency);
    }
    
    QueryPerformanceCounter((LARGE_INTEGER*)&g_TimerStart);
}

internal void hy_timer_reset()
{
    QueryPerformanceCounter((LARGE_INTEGER*)&g_TimerStart);
}

internal float hy_timer_get_milliseconds()
{
    double counter;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    float ms = (float)(1000.0f * (counter - g_TimerStart) / g_TimerFrequency);
    return ms;
}