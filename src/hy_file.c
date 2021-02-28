typedef struct
{
    void* data;
    u32 size;
} HyFile;


internal HyFile* HY_ReadFile(const char* fileName);
internal void HY_FreeFileMemory(void* memory);
internal b32 HY_WriteFile(char* fileName, u32 memorySize, void* memory);

///
///
///
internal HyFile* HY_ReadFile(const char* fileName)
{
    HyFile* result = VirtualAlloc(0, sizeof(HyFile), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle, &fileSize)) {
            u32 fileSize32 = SafeTruncateU64(fileSize.QuadPart);
            result->data = VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (result->data) {
                DWORD bytesRead;
                // NOTE(alex): the reason for the second check is because someone may truncate the file in between our GetFileSize() and ReadFile(). That would result.Data in a file size greater than the bytes read.
                if (ReadFile(fileHandle, result->data, fileSize32, &bytesRead, 0) && (fileSize32 == bytesRead)) {
                    result->size = fileSize32;
                }
                else {
                    // TODO(alex): logging
                    HY_FreeFileMemory(result->data);
                    result->data = 0;
                }
            }
            else {
                // TODO(alex): logging
            }
        }
        else {
            // TODO(alex): logging
        }
        
        CloseHandle(fileHandle);
    }
    else {
        // TODO(alex): logging
    }
    
    return result;
}

///
///
///
internal void HY_FreeFileMemory(void* memory)
{
    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

internal void HY_CloseFile(HyFile* file)
{
    VirtualFree(file->data, file->size, MEM_DECOMMIT | MEM_RELEASE);
    VirtualFree(file, sizeof(HyFile), MEM_DECOMMIT | MEM_RELEASE);
}

///
///
///
internal b32 HY_WriteFile(char* fileName, u32 memorySize, void* memory)
{
    b32 result = false;
    
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten;
        if (WriteFile(fileHandle, memory, memorySize, &bytesWritten, 0)) {
            result = (memorySize == bytesWritten);
        }
        else {
            // TODO(alex): logging
        }
        
        CloseHandle(fileHandle);
    }
    else {
        // TODO(alex): logging
    }
    
    return result;
}