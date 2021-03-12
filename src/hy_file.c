
/// Platform indipendent file.
typedef struct {
    char* data;
    u32   size;
} HyFile;

internal HyFile* hy_read_file(const char* fileName);
internal b32     hy_write_file(char* fileName, u32 memorySize, void* memory);

///
///
///
internal HyFile* hy_read_file(const char* fileName)
{
    char* data = NULL;
    u32 size = 0;
    
    // TODO(alex): Is all thi code necessary to read an entire file? Windows i guess...
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle, &fileSize)) {
            u32 fileSize32 = SafeTruncateU64(fileSize.QuadPart);
            data = hy_malloc(fileSize32 + 1);
            if (data) {
                DWORD bytesRead;
                // NOTE(alex): the reason for the second check is because someone may
                // truncate the file in between our GetFileSize() and ReadFile(). That
                // would result.Data in a file size greater than the bytes read.
                if (ReadFile(fileHandle, data, fileSize32, &bytesRead, 0) && (fileSize32 == bytesRead)) {
                    size = fileSize32;
                    data[size] = '\0';
                } else {
                    HY_WARN("Unable to read %s.", fileName);
                    hy_free(data);
                    data = 0;
                }
            } else {
                // TODO(alex): hy_malloc should be responsible of terminating the app in case of outofmemory
                HY_WARN("Unable to allocate memory for %s.", fileName);
            }
        } else {
            HY_WARN("Unable to retrieve file size of %s.", fileName);
        }
        
        CloseHandle(fileHandle);
    } else {
        HY_WARN("%s not found.", fileName);
        return NULL;
    }
    
    if (!data) {
        return NULL;
    }
    
    HyFile* result = hy_malloc(sizeof(HyFile));
    result->data = data;
    result->size = size;
    return result;
}

internal void HY_CloseFile(HyFile* file)
{
    if (file->data) {
        hy_free(file->data);
    }
    if (file) {
        hy_free(file);
    }
}

///
///
///
internal b32 hy_write_file(char* fileName, u32 memorySize, void* memory)
{
    b32 result = false;
    
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten;
        if (WriteFile(fileHandle, memory, memorySize, &bytesWritten, 0)) {
            result = (memorySize == bytesWritten);
        } else {
            // TODO(alex): logging
        }
        
        CloseHandle(fileHandle);
    } else {
        // TODO(alex): logging
    }
    
    return result;
}