typedef struct
{
    void* Data;
    u32 Size;
} DebugReadFileResult;


DebugReadFileResult DEBUGPlatformReadEntireFile(const char* fileName);
void DEBUGPlatformFreeFileMemory(void* memory);
b32 DEBUGPlatformWriteEntireFile(char* fileName, u32 memorySize, void* memory);

DebugReadFileResult DEBUGPlatformReadEntireFile(const char* fileName)
{
    DebugReadFileResult result = {0};
    
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle, &fileSize))
        {
            u32 fileSize32 = SafeTruncateU64(fileSize.QuadPart);
            result.Data = VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (result.Data)
            {
                DWORD bytesRead;
                // NOTE(alex): the reason for the second check is because someone may truncate the file in between our GetFileSize() and ReadFile(). That would result.Data in a file size greater than the bytes read.
                if (ReadFile(fileHandle, result.Data, fileSize32, &bytesRead, 0) && (fileSize32 == bytesRead))
                {
                    result.Size = fileSize32;
                }
                else
                {
                    // TODO(alex): logging
                    DEBUGPlatformFreeFileMemory(result.Data);
                    result.Data = 0;
                }
            }
            else
            {
                // TODO(alex): logging
            }
        }
        else
        {
            // TODO(alex): logging
        }
        
        CloseHandle(fileHandle);
    }
    else
    {
        // TODO(alex): logging
    }
    
    return result;
}

void DEBUGPlatformFreeFileMemory(void* memory)
{
    if (memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

b32 DEBUGPlatformWriteEntireFile(char* fileName, u32 memorySize, void* memory)
{
    b32 result = false;
    
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        if (WriteFile(fileHandle, memory, memorySize, &bytesWritten, 0))
        {
            result = (memorySize == bytesWritten);
        }
        else
        {
            // TODO(alex): logging
        }
        
        CloseHandle(fileHandle);
    }
    else
    {
        // TODO(alex): logging
    }
    
    return result;
}