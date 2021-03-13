/// @file
/// @brief Shared platform interface.
/// Every platform layer must implement this header.

#pragma once

/// @addtogroup Metrics
/// @{

internal float hy_calc_cpu_load(unsigned long long idleTicks, unsigned long long totalTicks);
internal float hy_get_cpu_load();

/// @}

/// @addtogroup Memory
/// @{

/// Malloc wrapper.
internal void* _hy_malloc(size_t nbytes, char* funcName);
/// Calloc wrapper.
internal void* _hy_calloc(size_t count, size_t nbytes, char* funcName);
/// Realloc wrapper.
internal void* _hy_realloc(void* p, size_t nbytes, char* funcName);
/// Free wrapper.
internal void _hy_free(void* p, char* funcName);

#define hy_malloc(nbytes)        _hy_malloc(nbytes, __FUNCTION__)
#define hy_calloc(count, nbytes) _hy_calloc(count, nbytes, __FUNCTION__)
#define hy_realloc(p, nbytes)    _hy_realloc(p, nbytes, __FUNCTION__)
#define hy_free(p)               _hy_free(p, __FUNCTION__)

/// @}