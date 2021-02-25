global_variable double g_TimerStart;
global_variable double g_TimerFrequency = 0.0f;

internal void HY_Timer_Init()
{
	if (g_TimerFrequency == 0.0f) {
		QueryPerformanceFrequency((LARGE_INTEGER*)&g_TimerFrequency);
	}
    
    QueryPerformanceCounter((LARGE_INTEGER*)&g_TimerStart);
}

internal void HY_Timer_Reset()
{
    QueryPerformanceCounter((LARGE_INTEGER*)&g_TimerStart);
}

internal float HY_Timer_GetMilliseconds()
{
    double counter;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
	float ms = (float)(1000.0f * (counter - g_TimerStart) / g_TimerFrequency);
    return ms;
}