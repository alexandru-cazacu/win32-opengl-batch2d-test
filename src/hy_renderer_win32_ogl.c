//~
// OpenGL Error handling
//

internal void hy_gl_clear_error()
{
    while (glGetError() != GL_NO_ERROR);
}

internal int hy_gl_log_call(const char* function, const char* file, int line)
{
    GLenum errorCode = glGetError();
    while (errorCode) {
        char* error = NULL;
        switch (errorCode) {
            case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW: error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW: error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        HY_ERROR("[GLLogCall] %d %s %s %s:%d\n", errorCode, error, function, file, line);
        return false;
    }
    return true;
}

#define ASSERT(x)                                                                                                      \
if (!(x)) {                                                                                                          \
HY_FATAL("Assert!\n");                                                                                             \
__debugbreak();                                                                                                    \
}
#define GL_CALL(x)                                                                                                     \
hy_gl_clear_error();                                                                                                      \
x;                                                                                                                   \
ASSERT(hy_gl_log_call(#x, __FILE__, __LINE__))

internal void APIENTRY hy_gl_debug_output(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
                                          const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
        return;
    }
    
    char* sourceMessage = NULL;
    switch (source) {
        case GL_DEBUG_SOURCE_API: sourceMessage = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceMessage = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceMessage = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: sourceMessage = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: sourceMessage = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER: sourceMessage = "Other"; break;
    }
    
    char* typeMessage = NULL;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: typeMessage = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeMessage = "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeMessage = "Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY: typeMessage = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: typeMessage = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER: typeMessage = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: typeMessage = "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: typeMessage = "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: typeMessage = "Other"; break;
    }
    
    char* severityMessage = NULL;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: severityMessage = "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: severityMessage = "medium"; break;
        case GL_DEBUG_SEVERITY_LOW: severityMessage = "low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severityMessage = "notification"; break;
    }
    
    HY_ERROR("[glDebugOutput] (%d): %s"
             "  Source: %s\n"
             "  Type: %s\n"
             "  Severity: %s\n",
             id, message, sourceMessage, typeMessage, severityMessage);
}

//~
// Vectors
//

typedef struct { float x, y; } V2;
typedef struct { float x, y, z; } V3;
typedef struct { float x, y, z, w; } V4;


//~ Textures

typedef struct {
    uint32_t    rendererID;
    uint32_t    width;
    uint32_t    height;
    uint32_t    channels;
    const char* path;
} HyTexture;

typedef enum HyTextureFilterMode { HyTextureFilterMode_Linear, HyTextureFilterMode_Nearest } HyTextureFilterMode;

internal HyTexture* hy_texture_create(const char* path, HyTextureFilterMode filter)
{
    // TODO(alex): Make Linear the default filter mode?
    
    HyTexture* result = hy_malloc(sizeof(HyTexture));
    
    // TODO(alex): Move in asset manager initialization.
    stbi_set_flip_vertically_on_load(true);
    
    uint32_t textureID;
    GL_CALL(glGenTextures(1, &textureID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, textureID));
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    
    if (filter == HyTextureFilterMode_Linear) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    if (filter == HyTextureFilterMode_Nearest) {
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    }
    
    // load and generate the texture
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t nrChannels = 0;
    unsigned char* data = stbi_load(path, &(int)width, &(int)height, &(int)nrChannels, 0);
    if (data) {
        uint32_t glChannels = nrChannels == 4 ? GL_RGBA : GL_RGB;
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, glChannels, GL_UNSIGNED_BYTE, data));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        
        result->rendererID = textureID;
        result->width = width;
        result->height = height;
        result->channels = nrChannels;
        result->path = path;
        
        stbi_image_free(data);
    } else { // Fallback debug texture.
        uint32_t dt[] = {
            0xff0000ff, // ABGR
            0xff0000ff,
            0xff0000ff,
            0xff0000ff,
        };
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, dt));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        
        result->rendererID = textureID;
        result->width = 2;
        result->height = 2;
        result->channels = 4;
        result->path = path;
    }
    
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    
    return result;
    
    // TODO(alex): Delete texture when?
}

internal void hy_texture_destroy(HyTexture* texture)
{
    GL_CALL(glDeleteTextures(1, &texture->rendererID));
}

internal void hy_texture_bind(HyTexture* texture, uint32_t location)
{
    GL_CALL(glActiveTexture(GL_TEXTURE0 + location));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->rendererID));
}

//~ Framebuffer

typedef struct {
    uint32_t rendererID;
    uint32_t colorAttachmentRendererID;
    uint32_t depthAttachmentRendererID;
    
    uint32_t width;
    uint32_t height;
} HyFramebuffer;

internal HyFramebuffer HyFramebuffer_Create(uint32_t width, uint32_t height);
internal void          HyFramebuffer_Destroy(HyFramebuffer* framebuffer);
internal void          HyFramebuffer_Resize(HyFramebuffer* framebuffer, uint32_t width, uint32_t height);
internal void          HyFramebuffer_Bind(HyFramebuffer* framebuffer);
internal void          HyFramebuffer_Unbind();

internal HyFramebuffer HyFramebuffer_Create(uint32_t width, uint32_t height)
{
    HyFramebuffer framebuffer = {0};
    
    HyFramebuffer_Resize(&framebuffer, width, height);
    
    return framebuffer;
}

internal void HyFramebuffer_Destroy(HyFramebuffer* framebuffer)
{
    GL_CALL(glDeleteFramebuffers(1, &framebuffer->rendererID));
    GL_CALL(glDeleteTextures(1, &framebuffer->colorAttachmentRendererID));
    // GL_CALL(glDeleteRenderbuffers(1, &framebuffer->depthAttachmentRendererID));
}

internal void HyFramebuffer_Resize(HyFramebuffer* framebuffer, uint32_t width, uint32_t height)
{
    if (framebuffer->width != width && framebuffer->height != height && framebuffer->rendererID) {
        HyFramebuffer_Destroy(framebuffer);
        GL_CALL(glDeleteTextures(1, &framebuffer->colorAttachmentRendererID));
        // GL_CALL(glDeleteRenderbuffers(1,
        // &framebuffer->depthAttachmentRendererID));
    }
    
    framebuffer->width = width;
    framebuffer->height = height;
    
    GL_CALL(glGenFramebuffers(1, &framebuffer->rendererID));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->rendererID));
    
    GL_CALL(glGenTextures(1, &framebuffer->colorAttachmentRendererID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, framebuffer->colorAttachmentRendererID));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebuffer->width, framebuffer->height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         NULL));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   framebuffer->colorAttachmentRendererID, 0));
    
#if 0    
    GL_CALL(glGenRenderbuffers(1, &framebuffer->depthAttachmentRendererID));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depthAttachmentRendererID));
    GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, framebuffer->width, framebuffer->height)); 
    GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depthAttachmentRendererID));
#endif
    
    HY_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete.");
    
    HyFramebuffer_Unbind();
}

internal void HyFramebuffer_Bind(HyFramebuffer* framebuffer)
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->rendererID));
    GL_CALL(glViewport(0, 0, framebuffer->width, framebuffer->height));
}

internal void HyFramebuffer_Unbind()
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

//~
/// Colors
///

/// RGBA float colors.
typedef struct {
    float r;
    float g;
    float b;
    float a;
} HyColor;

uint32_t white = 0xFFFFFFFF;
uint32_t bg0 = 0x282828FF;
uint32_t bg1 = 0x3C3836FF;
uint32_t bg0_s = 0x32302FFF;
uint32_t gray = 0x928374FF;
uint32_t red0 = 0xCC241dFF;
uint32_t red1 = 0xFB4939FF;
uint32_t green0 = 0x98971AFF;
uint32_t green1 = 0xB8BB26FF;
uint32_t purple0 = 0xB16286FF;
uint32_t purple1 = 0xD3869BFF;
uint32_t aqua0 = 0x689D6AFF;
uint32_t aqua1 = 0x8EC07CFF;
uint32_t fg = 0xebdbb2ff;

// TODO(alex): Remove?
internal HyColor HyWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
#if 0
internal float HyColor_Grey2[] = {0.2f, 0.2f, 0.2f, 1.0f};
internal float HyColor_Grey1[] = {0.1f, 0.1f, 0.1f, 1.0f};
internal float HyColor_Grey05[] = {0.05f, 0.05f, 0.05f, 1.0f};
internal float HyColor_Grey[] = {0.1f, 0.1f, 0.1f, 1.0f};
internal float HyColor_Black[] = {0.0f, 0.0f, 0.0f, 1.0f};
internal float HyColor_Red[] = {1.0f, 0.0f, 0.0f, 1.0f};
internal float HyColor_Green[] = {0.0f, 1.0f, 0.0f, 1.0f};
internal float HyColor_Blue[] = {0.0f, 0.0f, 1.0f, 1.0f};
internal float HyColor_Magenta[] = {1.0f, 0.0f, 1.0f, 1.0f};
internal float HyColor_Transparent[] = {0.0f, 0.0f, 0.0f, 0.0f};
#endif

internal HyColor hex_to_HyColor(uint32_t val)
{
    HyColor result = {0};
    result.r = ((val >> 24) & 0xff) / 255.0f;
    result.g = ((val >> 16) & 0xff) / 255.0f;
    result.b = ((val >> 8) & 0xff) / 255.0f;
    result.a = ((val) & 0xff) / 255.0f;
    
    return result;
}

//~
/// Camera
///

typedef struct {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 viewProjectionMatrix;
    
    vec3 position;
    vec3 rotation;
    vec3 scale;
    
    float fov;
    float nearPlane;
    float farPlane;
    
    vec3 worldUp;
    
    vec3 right;
    vec3 up;
    vec3 front;
} HyCamera;

//~ Buffer Element

typedef enum HyShaderDataType {
    HyShaderDataTypeNone,
    HyShaderDataTypeFloat,
    HyShaderDataTypeFloat2,
    HyShaderDataTypeFloat3,
    HyShaderDataTypeFloat4,
    HyShaderDataTypeMat3,
    HyShaderDataTypeMat4,
    HyShaderDataTypeInt,
    HyShaderDataTypeInt2,
    HyShaderDataTypeInt3,
    HyShaderDataTypeInt4,
    HyShaderDataTypeBool
} HyShaderDataType;

typedef struct {
    const char*      name;
    HyShaderDataType type;
    uint32_t         size;
    uint32_t         offset;
    BOOL             normalized;
} HyBufferElement;

typedef struct {
    HyBufferElement elements[16];
    uint32_t        elementsIndex;
    uint32_t        stride;
} HyBufferLayout;

// Returns the size in bytes.
internal uint32_t HY_ShaderDataType_GetSize(HyShaderDataType type)
{
    switch (type) {
        case HyShaderDataTypeFloat: return 4;
        case HyShaderDataTypeFloat2: return 4 * 2;
        case HyShaderDataTypeFloat3: return 4 * 3;
        case HyShaderDataTypeFloat4: return 4 * 4;
        case HyShaderDataTypeMat3: return 4 * 3 * 3;
        case HyShaderDataTypeMat4: return 4 * 4 * 4;
        case HyShaderDataTypeInt: return 4;
        case HyShaderDataTypeInt2: return 4 * 2;
        case HyShaderDataTypeInt3: return 4 * 3;
        case HyShaderDataTypeInt4: return 4 * 4;
        case HyShaderDataTypeBool: return 1;
    }
    
    HY_ASSERT(false, "Unknown HyShaderDataType!");
    return 0;
}

internal uint32_t HY_ShaderDataType_GetComponentCount(HyShaderDataType type)
{
    switch (type) {
        case HyShaderDataTypeFloat: return 1;
        case HyShaderDataTypeFloat2: return 2;
        case HyShaderDataTypeFloat3: return 3;
        case HyShaderDataTypeFloat4: return 4;
        case HyShaderDataTypeMat3: return 3 * 3;
        case HyShaderDataTypeMat4: return 4 * 4;
        case HyShaderDataTypeInt: return 1;
        case HyShaderDataTypeInt2: return 2;
        case HyShaderDataTypeInt3: return 3;
        case HyShaderDataTypeInt4: return 4;
        case HyShaderDataTypeBool: return 1;
    }
    
    HY_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}

internal GLenum HY_ShaderDataTypeToOpenGLBaseType(HyShaderDataType type)
{
    switch (type) {
        case HyShaderDataTypeFloat: return GL_FLOAT;
        case HyShaderDataTypeFloat2: return GL_FLOAT;
        case HyShaderDataTypeFloat3: return GL_FLOAT;
        case HyShaderDataTypeFloat4: return GL_FLOAT;
        case HyShaderDataTypeMat3: return GL_FLOAT;
        case HyShaderDataTypeMat4: return GL_FLOAT;
        case HyShaderDataTypeInt: return GL_INT;
        case HyShaderDataTypeInt2: return GL_INT;
        case HyShaderDataTypeInt3: return GL_INT;
        case HyShaderDataTypeInt4: return GL_INT;
        case HyShaderDataTypeBool: return GL_BOOL;
    }
    
    HY_ASSERT(false, "Unknown HyShaderDataType!");
    return 0;
}

internal void HY_BufferLayout_CalculateOffsetAndStride(HyBufferLayout* layout)
{
    uint32_t offset = 0;
    layout->stride = 0;
    for (uint32_t i = 0; i < 16; ++i) {
        layout->elements[i].offset = offset;
        offset += layout->elements[i].size;
        layout->stride += layout->elements[i].size;
    }
}

internal void HY_BufferLayout_PushElement(HyBufferLayout* layout, HyBufferElement element)
{
    layout->elements[layout->elementsIndex++] = element;
    
    HY_BufferLayout_CalculateOffsetAndStride(layout);
}

internal HyBufferElement HY_BufferElement_Create(HyShaderDataType dataType, const char* name, BOOL normalized)
{
    HyBufferElement bufferElement = {0};
    bufferElement.name = name;
    bufferElement.type = dataType;
    bufferElement.size = HY_ShaderDataType_GetSize(dataType);
    bufferElement.normalized = normalized;
    
    return bufferElement;
}

//~ Vertex buffer

typedef struct {
    HyBufferLayout layout;
    void*          vertices;
    uint32_t       size;
    uint32_t       rendererID;
} HyVertexBuffer;

// TODO(alex): Remove vertex buffer abstraction?
internal HyVertexBuffer HY_VertexBuffer_Create(float* vertices, uint32_t size)
{
    HyVertexBuffer vertexBuffer = {0};
    vertexBuffer.size = size;
    vertexBuffer.vertices = vertices;
    
    // GL_CALL(glCreateBuffers(1, &vertexBuffer.rendererID));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.rendererID));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW));
    
    return vertexBuffer;
}

internal void HY_DestroyVertexBuffer(HyVertexBuffer* vertexBuffer)
{
    GL_CALL(glDeleteBuffers(1, &vertexBuffer->rendererID));
}

internal void HY_SetVertexBufferLayout(HyVertexBuffer* vertexBuffer, HyBufferLayout* layout)
{
    vertexBuffer->layout = *layout;
}

internal void HY_SetVertexBufferData(HyVertexBuffer* vertexBuffer, void* data, uint32_t size)
{
    vertexBuffer->vertices = data;
    vertexBuffer->size = size;
    
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->rendererID));
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBuffer->size, vertexBuffer->vertices));
}

internal void HY_BindVertexBuffer(HyVertexBuffer* vertexBuffer)
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->rendererID));
}

internal void HY_UnbindVertexBuffer()
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

//~ Index buffer

typedef struct {
    uint32_t* indices;
    uint32_t  rendererID;
    uint32_t  count;
} HyIndexBuffer;

internal HyIndexBuffer HY_CreateIndexBuffer(uint32_t* indices, uint32_t count)
{
    HyIndexBuffer indexBuffer = {0};
    
    indexBuffer.indices = indices;
    indexBuffer.count = count;
    
    // GL_CALL(glCreateBuffers(1, &indexBuffer.rendererID));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.rendererID));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW));
    
    return indexBuffer;
}

internal void HY_DestroyIndexBuffer(HyIndexBuffer* indexBuffer)
{
    GL_CALL(glDeleteBuffers(1, &indexBuffer->rendererID));
}

internal void HY_BindIndexBuffer(HyIndexBuffer* indexBuffer)
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->rendererID));
}

internal void HY_UnbindIndexBuffer()
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

//~ Vertex array

typedef struct {
    HyVertexBuffer* vertexBuffers[8];
    HyIndexBuffer*  indexBuffer;
    uint32_t        rendererID;
    uint32_t        vertexBufferCount;
} HyVertexArray;

internal HyVertexArray HY_VertexArray_Create()
{
    HyVertexArray vertexArray = {0};
    
    GL_CALL(glGenVertexArrays(1, &vertexArray.rendererID));
    
    return vertexArray;
}

internal void HY_VertexArray_Destroy(HyVertexArray* vertexArray)
{
    // TODO(alex): Implement
}

internal void HY_VertexArray_AddVertexBuffer(HyVertexArray* vertexArray, HyVertexBuffer* vertexBuffer)
{
    HY_ASSERT(vertexBuffer->layout.elementsIndex, "Vertex Buffer has no layout!");
    
    GL_CALL(glBindVertexArray(vertexArray->rendererID));
    HY_BindVertexBuffer(vertexBuffer);
    
    uint32_t index = 0;
    
    HyBufferLayout layout = vertexBuffer->layout;
    
    for (uint32_t i = 0; i < layout.elementsIndex; ++i) {
        GL_CALL(glEnableVertexAttribArray(index));
        GL_CALL(glVertexAttribPointer(index, HY_ShaderDataType_GetComponentCount(layout.elements[i].type),
                                      HY_ShaderDataTypeToOpenGLBaseType(layout.elements[i].type),
                                      layout.elements[i].normalized ? GL_TRUE : GL_FALSE, layout.stride,
                                      (void*)(uint64_t)layout.elements[i].offset));
        index++;
    }
    
    vertexArray->vertexBuffers[vertexArray->vertexBufferCount++] = vertexBuffer;
}

internal void HY_VertexArray_SetIndexBuffer(HyVertexArray* vertexArray, HyIndexBuffer* indexBuffer)
{
    GL_CALL(glBindVertexArray(vertexArray->rendererID));
    HY_BindIndexBuffer(indexBuffer);
    
    vertexArray->indexBuffer = indexBuffer;
}

internal void HY_VertexArray_Bind(HyVertexArray* vertexArray)
{
    GL_CALL(glBindVertexArray(vertexArray->rendererID));
}

internal void HY_VertexArray_Unbind()
{
    GL_CALL(glBindVertexArray(0));
}

//~
/// Shader
///

#define HY_SHADER_MAP_SIZE 16

typedef struct {
    char name[128]; // TODO(alex): malloc
    uint32_t    id;
    
    int uniformLocationsMap[HY_SHADER_MAP_SIZE];
} HyShader;

typedef struct {
    int todo;
    // TODO(alex): Implement
} HyShaderLibrary;

internal void HY_Shader_CheckCompileErrors(uint32_t shader, const char* type)
{
    int  success;
    char infoLog[1024];
    if (strcmp(type, "PROGRAM") != 0) {
        GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
        if (!success) {
            GL_CALL(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
            HY_ERROR("%s", infoLog);
        }
    } else {
        GL_CALL(glGetProgramiv(shader, GL_LINK_STATUS, &success));
        if (!success) {
            GL_CALL(glGetProgramInfoLog(shader, 1024, NULL, infoLog));
            HY_ERROR("%s", infoLog);
        }
    }
}

internal HyShader* HY_Shader_Create(const char* vertFilePath, const char* fragFilePath)
{
    // TODO(alex): May fail.
    HyFile* vShaderCode = hy_read_file(vertFilePath);
    HyFile* fShaderCode = hy_read_file(fragFilePath);
    
    HyShader* shader = hy_malloc(sizeof(HyShader));
    
    // vertex Shader
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GL_CALL(glShaderSource(vertexShaderID, 1, &(char*)vShaderCode->data, (const GLint *)&vShaderCode->size));
    GL_CALL(glCompileShader(vertexShaderID));
    HY_Shader_CheckCompileErrors(vertexShaderID, "VERTEX");
    
    // fragment Shader
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GL_CALL(glShaderSource(fragmentShaderID, 1, &(char*)fShaderCode->data, (const GLint *)&fShaderCode->size));
    GL_CALL(glCompileShader(fragmentShaderID));
    HY_Shader_CheckCompileErrors(fragmentShaderID, "FRAGMENT");
    
    // shader Program
    shader->id = glCreateProgram();
    GL_CALL(glAttachShader(shader->id, vertexShaderID));
    GL_CALL(glAttachShader(shader->id, fragmentShaderID));
    GL_CALL(glLinkProgram(shader->id));
    HY_Shader_CheckCompileErrors(shader->id, "PROGRAM");
    
    // Delete the shaders as they're linked into our program now and no longer
    // necessary Delete alone won't delete a shader. You need to detach it first.
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDetachShader.xhtml
    GL_CALL(glDetachShader(shader->id, vertexShaderID));
    GL_CALL(glDetachShader(shader->id, fragmentShaderID));
    GL_CALL(glDeleteShader(vertexShaderID));
    GL_CALL(glDeleteShader(fragmentShaderID));
    
    // TODO(alex): Extract shader name.
    strncpy_s(shader->name, 128, vertFilePath, strlen(vertFilePath));
    
    // TODO(alex): What is this?
    for (int i = 0; i < HY_SHADER_MAP_SIZE; ++i) {
        shader->uniformLocationsMap[i] = -1;
    }
    
    return shader;
}

// Returns location if found, -1 otherwise.
internal int HY_Shader_GetUniformLocation(HyShader* shader, const char* name)
{
    // TODO(alex): Reimplement uniform hash table with collision check and correct
    // size.
    
#if 0
    unsigned long hash = HY_StringHash(name);
    
    int index = hash % HY_SHADER_MAP_SIZE;
    if (shader->uniformLocationsMap[index] == -1)
    {
        shader->uniformLocationsMap[index] = glGetUniformLocation(shader->id, name);
    }
    
    return shader->uniformLocationsMap[index];
#endif
    
    return glGetUniformLocation(shader->id, name);
}

internal void HY_Shader_Delete(HyShader* shader)
{
    GL_CALL(glDeleteProgram(shader->id));
}

internal void HY_Shader_Bind(HyShader* shader)
{
    GL_CALL(glUseProgram(shader->id));
}

internal void HY_Shader_SetInt(HyShader* shader, const char* name, int value)
{
    int loc = HY_Shader_GetUniformLocation(shader, name);
    
    GL_CALL(glUniform1i(loc, value));
}

internal void HY_Shader_SetFloat4(HyShader* shader, const char* name, const vec4 value)
{
    int loc = HY_Shader_GetUniformLocation(shader, name);
    
    // GL_CALL(glUniform4f(loc, value.x, value.y, value.z, value.w));
}

internal void HY_Shader_SetFloat3(HyShader* shader, const char* name, const vec3* value)
{
    int loc = HY_Shader_GetUniformLocation(shader, name);
    
    // GL_CALL(glUniform3f(loc, value->x, value->y, value->z));
}

internal void HY_Shader_SetFloat(HyShader* shader, const char* name, float value)
{
    int loc = HY_Shader_GetUniformLocation(shader, name);
    
    GL_CALL(glUniform1f(loc, value));
}

internal void HY_Shader_SetMat4(HyShader* shader, char* name, mat4 value)
{
    int loc = HY_Shader_GetUniformLocation(shader, name);
    
    if (loc != -1) {
        GL_CALL(glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)value));
    }
}

internal void hy_camera_update_vectors(HyCamera* camera, float aspectRatio)
{
    // Calculate the new Front vector
    vec3 direction;
    direction[0] = cos(glm_rad(camera->rotation[1])) * cos(glm_rad(camera->rotation[0]));
    direction[1] = sin(glm_rad(camera->rotation[0]));
    direction[2] = sin(glm_rad(camera->rotation[1])) * cos(glm_rad(camera->rotation[0]));
    glm_vec3_normalize_to(direction, camera->front);
    
    // Also re-calculate the Right and Up vector
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    vec3 fuCross;
    glm_cross(camera->front, camera->worldUp, fuCross);
    glm_normalize_to(fuCross, camera->right);
    vec3 rfCross;
    glm_cross(camera->right, camera->front, rfCross);
    glm_normalize_to(rfCross, camera->up);
    
    vec3 front;
    glm_vec3_add(camera->position, camera->front, front);
    glm_lookat(camera->position, front, camera->up, camera->viewMatrix);
    
    glm_perspective(glm_rad(camera->fov), aspectRatio, camera->nearPlane, camera->farPlane, camera->projectionMatrix);
    
#if 0
    // Calculate the new Front vector
    vec3 direction;
    direction[0] = cos(camera->rotation[1]) * cos(camera->rotation[0]);
    direction[1] = sin(camera->rotation[0]);
    direction[2] = sin(camera->rotation[1]) * cos(camera->rotation[0]);
    glm_vec3_normalize_to(direction, camera->front);
    
    // Also re-calculate the Right and Up vector
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    vec3 fuCross;
    glm_cross(camera->front, camera->worldUp, fuCross);
    glm_normalize_to(fuCross, camera->right);
    vec3 rfCross;
    glm_cross(camera->right, camera->front, rfCross);
    glm_normalize_to(rfCross, camera->up);
    
    vec3 front;
    glm_vec3_add(camera->position, camera->front, front);
    glm_lookat(camera->position, front, camera->up, camera->viewMatrix);
    
    glm_perspective(glm_rad(camera->fov), aspectRatio, camera->nearPlane, camera->farPlane, camera->projectionMatrix);
#endif
}

//~ Renderer2D

/// Camera 2D used for rendering.
typedef struct {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 viewProjectionMatrix;
    
    float width;
    float height;
    float nearPlane;
    float farPlane;
    
    vec4 viewport;
} HyCamera2D;

internal void HyCamera2D_Resize(HyCamera2D* camera, float width, float height, float nearPlane, float farPlane)
{
    camera->width = width;
    camera->height = height;
    camera->nearPlane = nearPlane;
    camera->farPlane = farPlane;
    
    glm_mat4_identity(camera->viewMatrix);
    glm_ortho(0.0f, width, 0.0f, height, nearPlane, farPlane, camera->projectionMatrix);
    glm_ortho(0.0f, width, 0.0f, height, nearPlane, farPlane, camera->viewProjectionMatrix);
    glm_vec4_copy((vec4){0.0f, 0.0f, width, height}, camera->viewport);
}

internal HyCamera2D HyCamera2D_Create(float width, float height, float nearPlane, float farPlane)
{
    HyCamera2D camera = {0};
    
    HyCamera2D_Resize(&camera, width, height, nearPlane, farPlane);
    
    return camera;
}

typedef struct {
    vec3  Pos;
    float TexIndex;
    HyColor Color;
    vec2  TexCoord;
} HyQuadVertex;

/// Batch 2D Renderer stats. Start/reset must be managed manually.
typedef struct {
    uint32_t drawCount;
    uint32_t quadCount;
} HyRenderer2DStats;

/// Batch 2D Renderer.
typedef struct {
    HyCamera2D* camera;
    
    uint32_t maxQuadCount;
    uint32_t maxVertexCount;
    uint32_t maxIndexCount;
    int32_t maxTextures;
    
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
    
    HyShader* textureShader;
    uint32_t  whiteTexture;
    uint32_t  whiteTextureSlot; // TODO(alex): Make 0 by default
    
    uint32_t quadIndexCount;
    
    HyQuadVertex* quadVertexBufferBase;
    HyQuadVertex* quadVertexBufferPtr;
    
    uint32_t textureSlots[32]; // TODO(alex): Maybe malloc ?
    uint32_t textureSlotIndex; // TODO(alex): Make 1 by default
    
    HyTexture* asciiTexture; // Debug text
    
    HyRenderer2DStats stats;
} HyRenderer2D;

global_variable HyRenderer2D g_renderer;

internal void hy_renderer2d_init();
internal void hy_renderer2d_shutdown();
internal void hy_renderer2d_begin_scene(HyCamera2D* camera);
internal void hy_renderer2d_end_scene();
internal void hy_renderer2d_flush();

internal HyRenderer2DStats hy_renderer2d_get_stats();
internal void hy_renderer2d_reset_stats();

internal HyRenderer2DStats hy_renderer2d_get_stats()
{
    return g_renderer.stats;
}

internal void hy_renderer2d_reset_stats()
{
    memset(&g_renderer.stats, 0, sizeof(HyRenderer2DStats));
}

internal void hy_renderer2d_init()
{
    HyRenderer2D* renderer = &g_renderer;
    
    HY_ASSERT(!renderer->quadVertexBufferBase, "Called hy_renderer2d_Init more than once.");
    
    // TODO(alex): Check if error callback is supported
    GL_CALL(glEnable(GL_DEBUG_OUTPUT));
    GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
    GL_CALL(glDebugMessageCallback(hy_gl_debug_output, NULL));
    GL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE));
    
    
    renderer->maxQuadCount = 10000;
    renderer->maxVertexCount = renderer->maxQuadCount * 4;
    renderer->maxIndexCount = renderer->maxQuadCount * 6;
    GL_CALL(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &renderer->maxTextures));
    
    renderer->quadVertexBufferBase = (HyQuadVertex*)hy_malloc(sizeof(HyQuadVertex) * renderer->maxVertexCount);
    
    GL_CALL(glGenVertexArrays(1, &renderer->vao));
    GL_CALL(glBindVertexArray(renderer->vao));
    
    GL_CALL(glGenBuffers(1, &renderer->vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, renderer->maxVertexCount * sizeof(HyQuadVertex), NULL, GL_DYNAMIC_DRAW));
    
    // Pos
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HyQuadVertex), (void*)(offsetof(HyQuadVertex, Pos))));
    GL_CALL(glEnableVertexAttribArray(0));
    // Color
    GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(HyQuadVertex), (void*)(offsetof(HyQuadVertex, Color))));
    GL_CALL(glEnableVertexAttribArray(1));
    // Tex coords
    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(HyQuadVertex), (void*)(offsetof(HyQuadVertex, TexCoord))));
    GL_CALL(glEnableVertexAttribArray(2));
    // Tex index
    GL_CALL(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(HyQuadVertex), (void*)(offsetof(HyQuadVertex, TexIndex))));
    GL_CALL(glEnableVertexAttribArray(3));
    
    uint32_t* indices = (uint32_t*)hy_malloc(sizeof(uint32_t) * renderer->maxIndexCount);
    uint32_t offset = 0;
    
    for (uint32_t i = 0; i < renderer->maxIndexCount; i += 6) {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;
        
        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;
        
        offset += 4;
    }
    
    GL_CALL(glGenBuffers(1, &renderer->ebo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ebo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * renderer->maxIndexCount, indices, GL_STATIC_DRAW));
    
    hy_free(indices);
    
    GL_CALL(glBindVertexArray(0));
    
    GL_CALL(glGenTextures(1, &renderer->whiteTexture));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, renderer->whiteTexture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    uint32_t color = 0xffffffff;
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color));
    
    renderer->textureShader = HY_Shader_Create("./assets/shaders/Batch 2D.vert", "./assets/shaders/Batch 2D.frag");
    HY_Shader_Bind(renderer->textureShader);
    
    renderer->whiteTextureSlot = 0;
    
    renderer->textureSlots[renderer->whiteTextureSlot] = renderer->whiteTexture;
    for (size_t i = 1; i < renderer->maxTextures; ++i) {
        renderer->textureSlots[i] = 0;
    }
    
    hy_renderer2d_reset_stats(renderer);
}

internal void hy_renderer2d_shutdown(HyRenderer2D* renderer)
{
    GL_CALL(glDeleteVertexArrays(1, &renderer->vao));
    GL_CALL(glDeleteBuffers(1, &renderer->vbo));
    GL_CALL(glDeleteBuffers(1, &renderer->ebo));
    
    hy_free(renderer->quadVertexBufferBase);
    
    GL_CALL(glDeleteTextures(1, &renderer->whiteTexture));
}

internal void hy_renderer2d_begin_scene(HyCamera2D* camera)
{
    HyRenderer2D* renderer = &g_renderer;
    
    if (camera) {
        renderer->camera = camera;
    }
    
    HY_Shader_Bind(renderer->textureShader);
    mat4 model;
    glm_mat4_identity(model);
    HY_Shader_SetMat4(renderer->textureShader, "u_Model", model);
    HY_Shader_SetMat4(renderer->textureShader, "u_View", renderer->camera->viewMatrix);
    HY_Shader_SetMat4(renderer->textureShader, "u_Projection", renderer->camera->projectionMatrix);
    
    renderer->quadIndexCount = 0;
    renderer->quadVertexBufferPtr = renderer->quadVertexBufferBase;
    renderer->textureSlotIndex = 1;
    
    int loc = glGetUniformLocation(renderer->textureShader->id, "u_Textures");
    int samplers[32]; // TODO(alex): Query max samplers.
    for (int i = 0; i < 32; ++i) {
        samplers[i] = i; // TODO(alex): zero?
    }
    GL_CALL(glUniform1iv(loc, 32, samplers));
}

internal void hy_renderer2d_end_scene()
{
    HyRenderer2D* renderer = &g_renderer;
    
    uint32_t size = (uint32_t)((uint8_t*)renderer->quadVertexBufferPtr - (uint8_t*)renderer->quadVertexBufferBase);
    
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo));
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, size, renderer->quadVertexBufferBase));
    
    hy_renderer2d_flush();
}

// TODO(alex): Search how to draw 1 million quads with memory mapped buffers.
// Currently we can do 200k quads @60fps.
internal void hy_renderer2d_flush()
{
    HyRenderer2D* renderer = &g_renderer;
    
    for (uint32_t i = 0; i < renderer->textureSlotIndex; ++i) {
        GL_CALL(glBindTextureUnit(i, renderer->textureSlots[i]));
        //GL_CALL(glActiveTexture(GL_TEXTURE0 + i));
        //GL_CALL(glBindTexture(GL_TEXTURE_2D, renderer->textureSlots[i]));
        // TODO(alex): What is the difference between the separate calls and the
        // single one?
    }
    
    GL_CALL(glBindVertexArray(renderer->vao));
    GL_CALL(glDrawElements(GL_TRIANGLES, renderer->quadIndexCount, GL_UNSIGNED_INT, NULL));
    renderer->stats.drawCount++;
}

internal void draw_quad_3tcc(vec3 pos, float width, float height, HyTexture* hyTexture, HyColor color, float tx,
                             float ty, float tw, float th)
{
    HyRenderer2D* renderer = &g_renderer;
    
    // Checks if we have room in our current batch for more quads.
    // 31 because the first one is a 1x1 white texture
    if (renderer->quadIndexCount >= renderer->maxIndexCount || renderer->textureSlotIndex > 31) {
        hy_renderer2d_end_scene();
        hy_renderer2d_begin_scene(NULL);
    }
    
    float textureIndex = 0.0f;
    // Skip first 1x1 white texture
    // hHecks if current texture was used to render another quad.
    for (uint32_t i = 1; i < renderer->textureSlotIndex; ++i) {
        if (renderer->textureSlots[i] == hyTexture->rendererID) {
            textureIndex = (float)i;
            break;
        }
    }
    
    // Didn't found in previous loop. Put in next available free texture slot.
    if (textureIndex == 0.0f) {
        textureIndex = (float)renderer->textureSlotIndex;
        renderer->textureSlots[renderer->textureSlotIndex] = hyTexture->rendererID;
        renderer->textureSlotIndex++;
    }
    
    glm_vec3_copy((vec3){pos[0], pos[1], pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){tx, ty}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    glm_vec3_copy((vec3){pos[0] + width, pos[1], pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){tx + tw, ty}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    glm_vec3_copy((vec3){pos[0] + width, pos[1] + height, pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){tx + tw, ty + th}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    glm_vec3_copy((vec3){pos[0], pos[1] + height, pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){tx, ty + th}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadIndexCount += 6;
    renderer->stats.quadCount++;
}

internal void draw_quad_3tc(vec3 pos, vec2 size, HyTexture* hyTexture, HyColor color)
{
    HyRenderer2D* renderer = &g_renderer;
    
    // Checks if we have room in our current batch for more quads.
    // 31 because the first one is a 1x1 white texture
    if (renderer->quadIndexCount >= renderer->maxIndexCount || renderer->textureSlotIndex > 31) {
        hy_renderer2d_end_scene();
        hy_renderer2d_begin_scene(NULL);
    }
    
    float textureIndex = 0.0f;
    // Skip first 1x1 white texture
    // hHecks if current texture was used to render another quad.
    for (uint32_t i = 1; i < renderer->textureSlotIndex; ++i) {
        if (renderer->textureSlots[i] == hyTexture->rendererID) {
            textureIndex = (float)i;
            break;
        }
    }
    
    // Didn't found in previous loop. Put in next available free texture slot.
    if (textureIndex == 0.0f) {
        textureIndex = (float)renderer->textureSlotIndex;
        renderer->textureSlots[renderer->textureSlotIndex] = hyTexture->rendererID;
        renderer->textureSlotIndex++;
    }
    
    glm_vec3_copy((vec3){pos[0], pos[1], pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){0.0f, 0.0f}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    glm_vec3_copy((vec3){pos[0] + size[0], pos[1], pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){1.0f, 0.0f}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    glm_vec3_copy((vec3){pos[0] + size[0], pos[1] + size[1], pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){1.0f, 1.0f}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    glm_vec3_copy((vec3){pos[0], pos[1] + size[1], pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){0.0f, 1.0f}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadIndexCount += 6;
    renderer->stats.quadCount++;
}

internal void draw_quad_3c(vec3 pos, vec2 size, HyColor color)
{
    HyRenderer2D* renderer = &g_renderer;
    
    // Checks if we have room in our current batch for more quads.
    // 31 because the first one is a 1x1 white texture
    if (renderer->quadIndexCount >= renderer->maxIndexCount) {
        hy_renderer2d_end_scene();
        hy_renderer2d_begin_scene(NULL);
    }
    
    glm_vec3_copy(pos, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){0.0f, 0.0f}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = (float)renderer->whiteTextureSlot;
    renderer->quadVertexBufferPtr++;
    
    glm_vec3_copy((vec3){pos[0] + size[0], pos[1], pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){1.0f, 0.0f}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = (float)renderer->whiteTextureSlot;
    renderer->quadVertexBufferPtr++;
    
    glm_vec3_copy((vec3){pos[0] + size[0], pos[1] + size[1], pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){1.0f, 1.0f}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = (float)renderer->whiteTextureSlot;
    renderer->quadVertexBufferPtr++;
    
    glm_vec3_copy((vec3){pos[0], pos[1] + size[1], pos[2]}, renderer->quadVertexBufferPtr->Pos);
    glm_vec2_copy((vec2){0.0f, 1.0f}, renderer->quadVertexBufferPtr->TexCoord);
    renderer->quadVertexBufferPtr->Color = color;
    renderer->quadVertexBufferPtr->TexIndex = (float)renderer->whiteTextureSlot;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadIndexCount += 6;
    renderer->stats.quadCount++;
}

internal void draw_quad_2tc(float x, float y, vec2 size, HyTexture* hyTexture, HyColor color)
{
    vec3 temp = { x, y, 1.0f };
    draw_quad_3tc(temp, size, hyTexture, color);
}

internal void draw_quad_2c(vec2 pos, vec2 size, HyColor color)
{
    vec3 tempPos = {pos[0], pos[1], 0.0f};
    draw_quad_3c(tempPos, size, color);
}

// TODO(alex): Clearly not a define, fine for debugging purposes
#define FONT_SIZE 18.0f

internal void draw_debug_text(const char* string, float x, float y, HyColor color)
{
    HyRenderer2D* renderer = &g_renderer;
    
    int   cellsPerRow = 10;
    int   cellsPerCol = 10;
    int   firstCharIndex = 31;
    float charWidth = FONT_SIZE;
    float charPad = -charWidth / 2.0f;
    float lineHeight = charWidth;
    HyColor currColor = color;
    
    char c = string[0];
    int  i = 0;
    int  xOffset = 0;
    while (c != '\0') {
        if (c == '\n') {
            y -= lineHeight;
            c = string[++i];
            xOffset = 0;
            currColor = color;
            continue;
        }
        if (c == '\r') {
            c = string[++i];
            continue;
        }
        if (c == '/') {
            currColor = hex_to_HyColor(gray);
        }
        if (c == '#') {
            currColor = hex_to_HyColor(purple1);
        }
        
        c -= (char)firstCharIndex;
        
        float cw = 1.0f / cellsPerRow;
        float ch = 1.0f / cellsPerCol;
        float cx = (float)((c % cellsPerRow) - 1) * cw;
        float cy = (cellsPerCol - (float)ceil((float)c / (float)cellsPerCol)) * ch;
        
        draw_quad_3tcc((vec3){x + (xOffset * (charWidth + charPad)), y, 0.0f},
                       charWidth, charWidth,
                       renderer->asciiTexture, currColor,
                       cx, cy, cw, ch);
        
        c = string[++i];
        xOffset++;
    }
}

internal void HY_SetClearColorCmd(HyColor* color)
{
    GL_CALL(glClearColor(color->r, color->g, color->b, color->a));
}

internal void HY_SetClearColorCmdByColors(float r, float g, float b, float a)
{
    GL_CALL(glClearColor(r, g, b, a));
}

internal void HY_ClearCmd()
{
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

internal void HY_ClearColorCmd()
{
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}
