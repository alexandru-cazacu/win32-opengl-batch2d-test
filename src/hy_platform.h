#pragma once

internal float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
internal unsigned long long FileTimeToInt64(FILETIME ft);
internal float GetCPULoad();

internal void* _hy_malloc(size_t nbytes, char* funcName);
internal void* _hy_calloc(size_t count, size_t nbytes, char* funcName);
internal void* _hy_realloc(void* p, size_t nbytes, char* funcName);
internal void _hy_free(void* p, char* funcName);

#define hy_malloc(nbytes)        _hy_malloc(nbytes, __FUNCTION__)
#define hy_calloc(count, nbytes) _hy_calloc(count, nbytes, __FUNCTION__)
#define hy_realloc(p, nbytes)    _hy_realloc(p, nbytes, __FUNCTION__)
#define hy_free(p)               _hy_free(p, __FUNCTION__)