//~ Error management

typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(GLenum target);
GLAPI PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap;
#define glGenerateMipmap glad_glGenerateMipmap
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
GLAPI PFNGLACTIVETEXTUREPROC glad_glActiveTexture;
#define glActiveTexture glad_glActiveTexture
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint *framebuffers);
GLAPI PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers;
#define glDeleteFramebuffers glad_glDeleteFramebuffers
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSPROC)(GLsizei n, const GLuint *renderbuffers);
GLAPI PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers;
#define glDeleteRenderbuffers glad_glDeleteRenderbuffers
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint *framebuffers);
GLAPI PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers;
#define glGenFramebuffers glad_glGenFramebuffers
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
GLAPI PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer;
#define glBindFramebuffer glad_glBindFramebuffer
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D;
#define glFramebufferTexture2D glad_glFramebufferTexture2D
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSPROC)(GLsizei n, GLuint *renderbuffers);
GLAPI PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers;
#define glGenRenderbuffers glad_glGenRenderbuffers
typedef void (APIENTRYP PFNGLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
GLAPI PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer;
#define glBindRenderbuffer glad_glBindRenderbuffer
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
GLAPI PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage;
#define glRenderbufferStorage glad_glRenderbufferStorage
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer;
#define glFramebufferRenderbuffer glad_glFramebufferRenderbuffer
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
GLAPI PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus;
#define glCheckFramebufferStatus glad_glCheckFramebufferStatus
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
GLAPI PFNGLGETSHADERIVPROC glad_glGetShaderiv;
#define glGetShaderiv glad_glGetShaderiv
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
GLAPI PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog;
#define glGetShaderInfoLog glad_glGetShaderInfoLog
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
GLAPI PFNGLGETPROGRAMIVPROC glad_glGetProgramiv;
#define glGetProgramiv glad_glGetProgramiv
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
GLAPI PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog;
#define glGetProgramInfoLog glad_glGetProgramInfoLog
typedef void (APIENTRYP PFNGLCREATEBUFFERSPROC)(GLsizei n, GLuint *buffers);
GLAPI PFNGLCREATEBUFFERSPROC glad_glCreateBuffers;
#define glCreateBuffers glad_glCreateBuffers
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
GLAPI PFNGLBINDBUFFERPROC glad_glBindBuffer;
#define glBindBuffer glad_glBindBuffer
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
GLAPI PFNGLBUFFERDATAPROC glad_glBufferData;
#define glBufferData glad_glBufferData
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
GLAPI PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers;
#define glDeleteBuffers glad_glDeleteBuffers
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
GLAPI PFNGLBUFFERSUBDATAPROC glad_glBufferSubData;
#define glBufferSubData glad_glBufferSubData
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
GLAPI PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays;
#define glGenVertexArrays glad_glGenVertexArrays
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint array);
GLAPI PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray;
#define glBindVertexArray glad_glBindVertexArray
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
#define glEnableVertexAttribArray glad_glEnableVertexAttribArray
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
GLAPI PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer;
#define glVertexAttribPointer glad_glVertexAttribPointer
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum type);
GLAPI PFNGLCREATESHADERPROC glad_glCreateShader;
#define glCreateShader glad_glCreateShader
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
GLAPI PFNGLSHADERSOURCEPROC glad_glShaderSource;
#define glShaderSource glad_glShaderSource
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shader);
GLAPI PFNGLCOMPILESHADERPROC glad_glCompileShader;
#define glCompileShader glad_glCompileShader
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)(void);
GLAPI PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
#define glCreateProgram glad_glCreateProgram
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
GLAPI PFNGLATTACHSHADERPROC glad_glAttachShader;
#define glAttachShader glad_glAttachShader
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint program);
GLAPI PFNGLLINKPROGRAMPROC glad_glLinkProgram;
#define glLinkProgram glad_glLinkProgram
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint shader);
GLAPI PFNGLDELETESHADERPROC glad_glDeleteShader;
#define glDeleteShader glad_glDeleteShader
typedef void (APIENTRYP PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
GLAPI PFNGLDETACHSHADERPROC glad_glDetachShader;
#define glDetachShader glad_glDetachShader
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
GLAPI PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
#define glGetUniformLocation glad_glGetUniformLocation
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint program);
GLAPI PFNGLDELETEPROGRAMPROC glad_glDeleteProgram;
#define glDeleteProgram glad_glDeleteProgram
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
GLAPI PFNGLUSEPROGRAMPROC glad_glUseProgram;
#define glUseProgram glad_glUseProgram
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
GLAPI PFNGLUNIFORM1IPROC glad_glUniform1i;
#define glUniform1i glad_glUniform1i
typedef void (APIENTRYP PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLAPI PFNGLUNIFORM4FPROC glad_glUniform4f;
#define glUniform4f glad_glUniform4f
typedef void (APIENTRYP PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GLAPI PFNGLUNIFORM3FPROC glad_glUniform3f;
#define glUniform3f glad_glUniform3f
typedef void (APIENTRYP PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
GLAPI PFNGLUNIFORM1FPROC glad_glUniform1f;
#define glUniform1f glad_glUniform1f
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv;
#define glUniformMatrix4fv glad_glUniformMatrix4fv
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
GLAPI PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
#define glDeleteVertexArrays glad_glDeleteVertexArrays
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
GLAPI PFNGLGENBUFFERSPROC glad_glGenBuffers;
#define glGenBuffers glad_glGenBuffers
typedef void (APIENTRYP PFNGLBINDTEXTUREUNITPROC)(GLuint unit, GLuint texture);
GLAPI PFNGLBINDTEXTUREUNITPROC glad_glBindTextureUnit;
#define glBindTextureUnit glad_glBindTextureUnit
typedef void (APIENTRYP PFNGLUNIFORM1IVPROC)(GLint location, GLsizei count, const GLint *value);
GLAPI PFNGLUNIFORM1IVPROC glad_glUniform1iv;
#define glUniform1iv glad_glUniform1iv
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
GLAPI PFNGLDRAWELEMENTSPROC glad_glDrawElements;
#define glDrawElements glad_glDrawElements
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLPROC)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
GLAPI PFNGLDEBUGMESSAGECONTROLPROC glad_glDebugMessageControl;
#define glDebugMessageControl glad_glDebugMessageControl
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, const void *userParam);
GLAPI PFNGLDEBUGMESSAGECALLBACKPROC glad_glDebugMessageCallback;
#define glDebugMessageCallback glad_glDebugMessageCallback

static void GLClearError()
{
    while(glGetError() != GL_NO_ERROR);
}

static int GLLogCall(const char* function, const char* file, int line)
{
    GLenum errorCode = glGetError();
    while(errorCode) {
        char* error = NULL;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        char msg[256];
        // TODO(alex): Fix, make a usable logging function.
        //OutputDebugStringA(msg, 256, "[GLLogCall] %d %s %s %s:%d\n", errorCode, error, function, file, line);
        OutputDebugStringA(msg);
        return false;
    }
    return true;
}

#define ASSERT(x) if (!(x)) { printf("Assert!\n"); __debugbreak(); }
#define GL_CALL(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    {
        return;
    }
    
    char* sourceMessage = NULL;
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             sourceMessage = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceMessage = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceMessage = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceMessage = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     sourceMessage = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           sourceMessage = "Other"; break;
    }
    
    char* typeMessage = NULL;
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               typeMessage = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeMessage = "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeMessage = "Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         typeMessage = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         typeMessage = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              typeMessage = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          typeMessage = "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           typeMessage = "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               typeMessage = "Other"; break;
    }
    
    char* severityMessage = NULL;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         severityMessage = "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       severityMessage = "medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          severityMessage = "low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severityMessage = "notification"; break;
    }
    
    char msg[256];
    // TODO(alex): Fix, make usable logging function.
#if 0
    snprintf(msg, 256, "[glDebugOutput] (%d): %s\n"
             "  Source: %s\n"
             "  Type: %s\n"
             "  Severity: %s\n", id, message, sourceMessage, typeMessage, severityMessage);
#endif
    OutputDebugStringA(msg);
}

//~ Colors

static float HyColor_White[] =       { 1.0f, 1.0f, 1.0f, 1.0f };
static float HyColor_Grey2[] =       { 0.2f, 0.2f, 0.2f, 1.0f };
static float HyColor_Grey1[] =       { 0.1f, 0.1f, 0.1f, 1.0f };
static float HyColor_Grey05[] =      { 0.05f, 0.05f, 0.05f, 1.0f };
static float HyColor_Grey[] =        { 0.1f, 0.1f, 0.1f, 1.0f };
static float HyColor_Black[] =       { 0.0f, 0.0f, 0.0f, 1.0f };
static float HyColor_Red[] =         { 1.0f, 0.0f, 0.0f, 1.0f };
static float HyColor_Green[] =       { 0.0f, 1.0f, 0.0f, 1.0f };
static float HyColor_Blue[] =        { 0.0f, 0.0f, 1.0f, 1.0f };
static float HyColor_Magenta[] =     { 1.0f, 0.0f, 1.0f, 1.0f };
static float HyColor_Transparent[] = { 0.0f, 0.0f, 0.0f, 0.0f };

//~ Textures

typedef struct
{
    uint32_t rendererID;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    const char* path;
} HyTexture;

typedef enum HyTextureFilterMode
{
    Linear,
    Nearest
} HyTextureFilterMode;

static int HyTexture_Create(HyTexture* texture, const char* path, HyTextureFilterMode filter)
{
    // TODO(alex): Make Linear the default filter mode?
    
    // TODO(alex): Move in asset manager initialization.
    stbi_set_flip_vertically_on_load(true);
    
    int result = 1;
    unsigned int textureID;
    GL_CALL(glGenTextures(1, &textureID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, textureID));
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    
    if (filter == Linear)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    if (filter == Nearest)
    {
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    }
    
    // load and generate the texture
    uint32_t width, height, nrChannels = 0;
    unsigned char *data = stbi_load(path, &(int)width, &(int)height, &(int)nrChannels, 0);
    if (data)
    {
        uint32_t glChannels = nrChannels == 4 ? GL_RGBA : GL_RGB;
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, glChannels, GL_UNSIGNED_BYTE, data));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        
        texture->rendererID = textureID;
        texture->width = width;
        texture->height = height;
        texture->channels = nrChannels;
        texture->path = path;
    }
    else
    {
        uint32_t dt[] = {
            0xff000000,
            0xffffffff,
            0xffffffff,
            0xff000000,
        };
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, dt));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        
        texture->rendererID = textureID;
        texture->width = 2;
        texture->height = 2;
        texture->channels = 4;
        texture->path = path;
        
        // TODO(alex): First read data, then create GPU info
        // TODO(alex): Delete texture
        result = -1;
    }
    stbi_image_free(data);
    
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    
    return result;
    
    // TODO(alex): Delete texture when?
}

static void HyTexture_Bind(HyTexture* texture, uint32_t location)
{
    GL_CALL(glActiveTexture(GL_TEXTURE0 + location));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->rendererID));
}

//~ Framebuffer

typedef struct
{
    uint32_t rendererID;
    uint32_t colorAttachmentRendererID;
    uint32_t depthAttachmentRendererID;
    
    uint32_t width;
    uint32_t height;
} HyFramebuffer;

static HyFramebuffer HyFramebuffer_Create(uint32_t width, uint32_t height);
static void HyFramebuffer_Destroy(HyFramebuffer* framebuffer);
static void HyFramebuffer_Resize(HyFramebuffer* framebuffer, uint32_t width, uint32_t height);
static void HyFramebuffer_Bind(HyFramebuffer* framebuffer);
static void HyFramebuffer_Unbind();

static HyFramebuffer HyFramebuffer_Create(uint32_t width, uint32_t height)
{
    HyFramebuffer framebuffer = {0};
    
    HyFramebuffer_Resize(&framebuffer, width, height);
    
    return framebuffer;
}

static void HyFramebuffer_Destroy(HyFramebuffer* framebuffer)
{
    GL_CALL(glDeleteFramebuffers(1, &framebuffer->rendererID));
    GL_CALL(glDeleteTextures(1,&framebuffer->colorAttachmentRendererID));
    GL_CALL(glDeleteRenderbuffers(1, &framebuffer->depthAttachmentRendererID));
}

static void HyFramebuffer_Resize(HyFramebuffer* framebuffer, uint32_t width, uint32_t height)
{
    if (framebuffer->width != width && framebuffer->height != height && framebuffer->rendererID)
    {
        HyFramebuffer_Destroy(framebuffer);
        GL_CALL(glDeleteTextures(1,&framebuffer->colorAttachmentRendererID));
        GL_CALL(glDeleteRenderbuffers(1, &framebuffer->depthAttachmentRendererID));
    }
    
    framebuffer->width = width;
    framebuffer->height = height;
    
    GL_CALL(glGenFramebuffers(1, &framebuffer->rendererID));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->rendererID));
    
    GL_CALL(glGenTextures(1, &framebuffer->colorAttachmentRendererID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, framebuffer->colorAttachmentRendererID));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebuffer->width, framebuffer->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->colorAttachmentRendererID, 0));
    
    GL_CALL(glGenRenderbuffers(1, &framebuffer->depthAttachmentRendererID));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depthAttachmentRendererID));
    GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, framebuffer->width, framebuffer->height)); 
    GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depthAttachmentRendererID));
    
    HY_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete.");
    
    HyFramebuffer_Unbind();
}

static void HyFramebuffer_Bind(HyFramebuffer* framebuffer)
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->rendererID));
    GL_CALL(glViewport(0, 0, framebuffer->width, framebuffer->height));
}

static void HyFramebuffer_Unbind()
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

//~ Camera

typedef struct
{
    // TODO(alex): Mat not vec
    float x;
    float y;
    float z;
    float w;
} HyMat4;

typedef struct
{
    float x;
    float y;
} HyVec2;

typedef struct
{
    float x;
    float y;
    float z;
} HyVec3;

typedef struct
{
    float x;
    float y;
    float z;
    float w;
} HyVec4;

typedef struct
{
    float r;
    float g;
    float b;
    float a;
} HyColor;

typedef struct
{
    HyMat4 projectionMatrix;
    HyMat4 viewMatrix;
    HyMat4 viewProjectionMatrix;
    
    HyVec3 position;
    HyVec3 rotation;
    HyVec3 scale;
    
    float fov;
    float nearPlane;
    float farPlane;
    
    HyVec3 worldUp;
    
    HyVec3 right;
    HyVec3 up;
    HyVec3 front;
} HyCamera;

//~ Buffer Element

typedef enum HyShaderDataType
{
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

typedef struct
{
    const char* name;
    HyShaderDataType type;
    uint32_t size;
    uint32_t offset;
    BOOL normalized;
} HyBufferElement;

typedef struct
{
    HyBufferElement elements[16];
    uint32_t elementsIndex;
    uint32_t stride;
} HyBufferLayout;

// Returns the size in bytes.
static uint32_t HY_ShaderDataType_GetSize(HyShaderDataType type)
{
    switch (type)
    {
		case HyShaderDataTypeFloat:   return 4;
		case HyShaderDataTypeFloat2:  return 4 * 2;
		case HyShaderDataTypeFloat3:  return 4 * 3;
		case HyShaderDataTypeFloat4:  return 4 * 4;
		case HyShaderDataTypeMat3:    return 4 * 3 * 3;
		case HyShaderDataTypeMat4:    return 4 * 4 * 4;
		case HyShaderDataTypeInt:     return 4;
		case HyShaderDataTypeInt2:    return 4 * 2;
		case HyShaderDataTypeInt3:    return 4 * 3;
		case HyShaderDataTypeInt4:    return 4 * 4;
		case HyShaderDataTypeBool:    return 1;
    }
    
    HY_ASSERT(false, "Unknown HyShaderDataType!");
    return 0;
}

static uint32_t HY_ShaderDataType_GetComponentCount(HyShaderDataType type)
{
    switch (type)
    {
        case HyShaderDataTypeFloat:     return 1;
        case HyShaderDataTypeFloat2:    return 2;
        case HyShaderDataTypeFloat3:    return 3;
        case HyShaderDataTypeFloat4:    return 4;
        case HyShaderDataTypeMat3:      return 3 * 3;
        case HyShaderDataTypeMat4:      return 4 * 4;
        case HyShaderDataTypeInt:       return 1;
        case HyShaderDataTypeInt2:      return 2;
        case HyShaderDataTypeInt3:      return 3;
        case HyShaderDataTypeInt4:      return 4;
        case HyShaderDataTypeBool:      return 1;
    }
    
    HY_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}

static GLenum HY_ShaderDataTypeToOpenGLBaseType(HyShaderDataType type)
{
    switch (type)
    {
		case HyShaderDataTypeFloat:     return GL_FLOAT;
		case HyShaderDataTypeFloat2:    return GL_FLOAT;
		case HyShaderDataTypeFloat3:    return GL_FLOAT;
		case HyShaderDataTypeFloat4:    return GL_FLOAT;
		case HyShaderDataTypeMat3:      return GL_FLOAT;
		case HyShaderDataTypeMat4:      return GL_FLOAT;
		case HyShaderDataTypeInt:       return GL_INT;
		case HyShaderDataTypeInt2:      return GL_INT;
		case HyShaderDataTypeInt3:      return GL_INT;
		case HyShaderDataTypeInt4:      return GL_INT;
		case HyShaderDataTypeBool:      return GL_BOOL;
    }
    
    HY_ASSERT(false, "Unknown HyShaderDataType!");
    return 0;
}

static void HY_BufferLayout_CalculateOffsetAndStride(HyBufferLayout* layout)
{
    uint32_t offset = 0;
    layout->stride = 0;
    for (uint32_t i = 0; i < 16; ++i)
    {
        layout->elements[i].offset = offset;
        offset += layout->elements[i].size;
        layout->stride += layout->elements[i].size;
    }
}

static void HY_BufferLayout_PushElement(HyBufferLayout* layout, HyBufferElement element)
{
    layout->elements[layout->elementsIndex++] = element;
    
    HY_BufferLayout_CalculateOffsetAndStride(layout);
}

static HyBufferElement HY_BufferElement_Create(HyShaderDataType dataType, const char* name, BOOL normalized)
{
    // TODO(alex): Make normalized default to false;
    normalized = false;
    
    HyBufferElement bufferElement = {0};
    bufferElement.name = name;
    bufferElement.type = dataType;
    bufferElement.size = HY_ShaderDataType_GetSize(dataType);
    bufferElement.normalized = normalized;
    
    return bufferElement;
}

static void HY_Shader_CheckCompileErrors(uint32_t shader, const char* type)
{
    int success;
    char infoLog[1024];
    if (strcmp(type, "PROGRAM") != 0)
    {
        GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
        if (!success)
        {
            GL_CALL(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
            HY_ERROR("{}", infoLog);
        }
    }
    else
    {
        GL_CALL(glGetProgramiv(shader, GL_LINK_STATUS, &success));
        if (!success)
        {
            GL_CALL(glGetProgramInfoLog(shader, 1024, NULL, infoLog));
            HY_ERROR("{}", infoLog);
        }
    }
}

//~ Vertex buffer

typedef struct
{
    HyBufferLayout layout;
    void* vertices;
    uint32_t size;
    uint32_t rendererID;
} HyVertexBuffer;

// TODO(alex): Remove vertex buffer abstraction?
static HyVertexBuffer HY_VertexBuffer_Create(float* vertices, uint32_t size)
{
    HyVertexBuffer vertexBuffer = {0};
    vertexBuffer.size = size;
    vertexBuffer.vertices = vertices;
    
    GL_CALL(glCreateBuffers(1, &vertexBuffer.rendererID));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.rendererID));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW));
    
    return vertexBuffer;
}

static void HY_DestroyVertexBuffer(HyVertexBuffer* vertexBuffer)
{
    GL_CALL(glDeleteBuffers(1, &vertexBuffer->rendererID));
}

static void HY_SetVertexBufferLayout(HyVertexBuffer* vertexBuffer, HyBufferLayout* layout)
{
    vertexBuffer->layout = *layout;
}

static void HY_SetVertexBufferData(HyVertexBuffer* vertexBuffer, void* data, uint32_t size)
{
    vertexBuffer->vertices = data;
    vertexBuffer->size = size;
    
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->rendererID));
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBuffer->size, vertexBuffer->vertices));
}

static void HY_BindVertexBuffer(HyVertexBuffer* vertexBuffer)
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->rendererID));
}

static void HY_UnbindVertexBuffer()
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

//~ Index buffer

typedef struct
{
    uint32_t* indices;
    uint32_t rendererID;
    uint32_t count;
} HyIndexBuffer;

static HyIndexBuffer HY_CreateIndexBuffer(uint32_t* indices, uint32_t count)
{
    HyIndexBuffer indexBuffer = {0};
    
    indexBuffer.indices = indices;
    indexBuffer.count = count;
    
    GL_CALL(glCreateBuffers(1, &indexBuffer.rendererID));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.rendererID));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW));
    
    return indexBuffer;
}

static void HY_DestroyIndexBuffer(HyIndexBuffer* indexBuffer)
{
    GL_CALL(glDeleteBuffers(1, &indexBuffer->rendererID));
}

static void HY_BindIndexBuffer(HyIndexBuffer* indexBuffer)
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->rendererID));
}

static void HY_UnbindIndexBuffer()
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

//~ Vertex array

typedef struct
{
    HyVertexBuffer* vertexBuffers[8];
    HyIndexBuffer* indexBuffer;
    uint32_t rendererID;
    uint32_t vertexBufferCount;
} HyVertexArray;

static HyVertexArray HY_VertexArray_Create()
{
    HyVertexArray vertexArray = {0};
    
    GL_CALL(glGenVertexArrays(1, &vertexArray.rendererID));
    
    return vertexArray;
}

static void HY_VertexArray_Destroy(HyVertexArray* vertexArray)
{
    // TODO(alex): Implement
}

static void HY_VertexArray_AddVertexBuffer(HyVertexArray* vertexArray, HyVertexBuffer* vertexBuffer)
{
    HY_ASSERT(vertexBuffer->layout.elementsIndex, "Vertex Buffer has no layout!");
    
    GL_CALL(glBindVertexArray(vertexArray->rendererID));
    HY_BindVertexBuffer(vertexBuffer);
    
    uint32_t index = 0;
    
    HyBufferLayout layout = vertexBuffer->layout;
    
    for (uint32_t i = 0; i < layout.elementsIndex; ++i)
    {
        GL_CALL(glEnableVertexAttribArray(index));
        GL_CALL(glVertexAttribPointer(index,
                                      HY_ShaderDataType_GetComponentCount(layout.elements[i].type),
                                      HY_ShaderDataTypeToOpenGLBaseType(layout.elements[i].type),
                                      layout.elements[i].normalized ? GL_TRUE : GL_FALSE,
                                      layout.stride,
                                      (void *)(uint64_t)layout.elements[i].offset));
        index++;
    }
    
    vertexArray->vertexBuffers[vertexArray->vertexBufferCount++] = vertexBuffer;
}

static void HY_VertexArray_SetIndexBuffer(HyVertexArray* vertexArray, HyIndexBuffer* indexBuffer)
{
    GL_CALL(glBindVertexArray(vertexArray->rendererID));
    HY_BindIndexBuffer(indexBuffer);
    
    vertexArray->indexBuffer = indexBuffer;
}

static void HY_VertexArray_Bind(HyVertexArray* vertexArray)
{
    GL_CALL(glBindVertexArray(vertexArray->rendererID));
}

static void HY_VertexArray_Unbind()
{
    GL_CALL(glBindVertexArray(0));
}

//~ Shader

#define HY_SHADER_MAP_SIZE 16

typedef struct
{
    const char* name;
    uint32_t id;
    
    int uniformLocationsMap[HY_SHADER_MAP_SIZE];
} HyShader;

typedef struct
{
    int todo;
    // TODO(alex): Implement
} HyShaderLibrary;

static HyShader* HY_Shader_Create(const char* vertFilePath, const char* fragFilePath)
{
    DebugReadFileResult vShaderCode = DEBUGPlatformReadEntireFile(vertFilePath);
    DebugReadFileResult fShaderCode = DEBUGPlatformReadEntireFile(fragFilePath);
    
    HyShader* shader = (HyShader*)malloc(sizeof (HyShader));
    
    // 2. compile shaders
    //int success;
    //char infoLog[512];
    
    // vertex Shader
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GL_CALL(glShaderSource(vertexShaderID, 1, &(char*)vShaderCode.Data, NULL));
    GL_CALL(glCompileShader(vertexShaderID));
    HY_Shader_CheckCompileErrors(vertexShaderID, "VERTEX");
    
    // fragment Shader
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GL_CALL(glShaderSource(fragmentShaderID, 1, &(char*)fShaderCode.Data, NULL));
    GL_CALL(glCompileShader(fragmentShaderID));
    HY_Shader_CheckCompileErrors(fragmentShaderID, "FRAGMENT");
    
    // shader Program
    shader->id = glCreateProgram();
    GL_CALL(glAttachShader(shader->id, vertexShaderID));
    GL_CALL(glAttachShader(shader->id, fragmentShaderID));
    GL_CALL(glLinkProgram(shader->id));
    HY_Shader_CheckCompileErrors(shader->id, "PROGRAM");
    
    // Delete the shaders as they're linked into our program now and no longer necessary
    // Delete alone won't delete a shader. You need to detach it first.
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDetachShader.xhtml
    GL_CALL(glDetachShader(shader->id , vertexShaderID));
    GL_CALL(glDetachShader(shader->id , fragmentShaderID));
    GL_CALL(glDeleteShader(vertexShaderID));
    GL_CALL(glDeleteShader(fragmentShaderID));
    
    shader->name = "test\n";
    
    for (int i = 0; i < HY_SHADER_MAP_SIZE; ++i)
    {
        shader->uniformLocationsMap[i] = -1;
    }
    
    return shader;
}

// Returns location if found, -1 otherwise.
static int HY_Shader_GetUniformLocation(HyShader* shader, const char* name)
{
    // TODO(alex): Reimplement uniform hash table with collision check and correct size.
    
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

static void HY_Shader_Delete(HyShader* shader)
{
    GL_CALL(glDeleteProgram(shader->id));
}

static void HY_Shader_Bind(HyShader* shader)
{
    GL_CALL(glUseProgram(shader->id));
}

static void HY_Shader_SetInt(HyShader* shader, const char* name, int value)
{
    int loc = HY_Shader_GetUniformLocation(shader, name);
    
    GL_CALL(glUniform1i(loc, value));
}

static void HY_Shader_SetFloat4(HyShader* shader, const char* name, const HyVec4 value)
{
    int loc = HY_Shader_GetUniformLocation(shader, name);
    
    GL_CALL(glUniform4f(loc, value.x, value.y, value.z, value.w));
}

static void HY_Shader_SetFloat3(HyShader* shader, const char* name, const HyVec3* value)
{
    int loc = HY_Shader_GetUniformLocation(shader, name);
    
    GL_CALL(glUniform3f(loc, value->x, value->y, value->z));
}

static void HY_Shader_SetFloat(HyShader* shader, const char* name, float value)
{
    int loc = HY_Shader_GetUniformLocation(shader, name);
    
    GL_CALL(glUniform1f(loc, value));
}

static void HY_Shader_SetMat4(HyShader* shader, const char* name, const HyMat4* value)
{
    int loc = HY_Shader_GetUniformLocation(shader, name);
    
    if (loc != -1)
    {
        GL_CALL(glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)value));
    }
}

#if 0
static void HyCamera_UpdateVectors(HyCamera* camera, float aspectRatio)
{
    // Calculate the new Front vector
    glm::vec3 direction;
    direction.x = cos(glm::radians(camera.rotation.y)) * cos(glm::radians(camera.rotation.x));
    direction.y = sin(glm::radians(camera.rotation.x));
    direction.z = sin(glm::radians(camera.rotation.y)) * cos(glm::radians(camera.rotation.x));
    camera.front = glm::normalize(direction);
    
    // Also re-calculate the Right and Up vector
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    camera.right = glm::normalize(glm::cross(camera.front, camera.worldUp));
    camera.up = glm::normalize(glm::cross(camera.right, camera.front));
    
    camera.viewMatrix = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
    
    camera.projectionMatrix = glm::perspective(glm::radians(camera.fov), aspectRatio, camera.nearPlane, camera.farPlane);
}
#endif

//~ Renderer 2D

typedef struct
{
    HyMat4 projectionMatrix;
    HyMat4 viewMatrix;
    HyMat4 viewProjectionMatrix;
    
    float width;
    float height;
    float nearPlane;
    float farPlane;
    
    HyVec4 viewport;
} HyCamera2D;

static void HyCamera2D_Resize(HyCamera2D *camera, float width, float height, float nearPlane, float farPlane);

static HyCamera2D HyCamera2D_Create(float width, float height, float nearPlane, float farPlane)
{
    HyCamera2D camera = {0};
    
    HyCamera2D_Resize(&camera, width, height, nearPlane, farPlane);
    
    return camera;
}

static void HyCamera2D_Resize(HyCamera2D *camera, float width, float height, float nearPlane, float farPlane)
{
    camera->width = width;
    camera->height = height;
    camera->nearPlane = nearPlane;
    camera->farPlane = farPlane;
    
    // TODO(alex): Implement
#if 0
    camera->viewMatrix = glm::mat4(1.0f);
    camera->projectionMatrix = glm::ortho(0.0f, width, 0.0f, height, nearPlane, farPlane);
    camera->viewProjectionMatrix = glm::ortho(0.0f, width, 0.0f, height, nearPlane, farPlane);
    
    camera->viewport = glm::vec4 { 0.0f, 0.0f, width, height };
#endif
}

typedef struct
{
    HyVec3 Pos;
    HyVec4 Color;
    HyVec2 TexCoord;
    float TexIndex;
} HyQuadVertex;

typedef struct
{
    uint32_t drawCount;
    uint32_t quadCount;
} HyRenderer2DStats;

typedef struct
{
    HyCamera2D* camera;
    
    uint32_t maxQuadCount;
    uint32_t maxVertexCount;
    uint32_t maxIndexCount;
    uint32_t maxTextures;
    
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
    
    HyShader* textureShader;
    uint32_t whiteTexture;
    uint32_t whiteTextureSlot; // TODO(alex): Make 0 by default
    
    uint32_t quadIndexCount;
    
    HyQuadVertex* quadVertexBufferBase;
    HyQuadVertex* quadVertexBufferPtr;
    
    uint32_t textureSlots[32]; // TODO(alex): Maybe malloc ?
    uint32_t textureSlotIndex; // TODO(alex): Make 1 by default
    
    HyRenderer2DStats stats;
} HyRenderer2D;

static void HyRenderer2D_Init(HyRenderer2D* renderer);
static void HyRenderer2D_Shutdown(HyRenderer2D* renderer);
//static void HyRenderer2D_BeginScene(HyRenderer2D* renderer, HyCamera2D* camera);
static void HyRenderer2D_EndScene(HyRenderer2D* renderer);
static void HyRenderer2D_Flush(HyRenderer2D* renderer);

static HyRenderer2DStats HyRenderer2D_GetStats(HyRenderer2D* renderer);
static void HY_Renderer2D_ResetStats(HyRenderer2D* renderer);

static void DrawQuad3C(HyRenderer2D* renderer, const HyVec3* pos, const HyVec2* size, const HyVec4* color);
static void DrawQuad2C(HyRenderer2D* renderer, const HyVec2* pos, const HyVec2* size, const HyVec4* color);
//static void DrawQuad3T(HyRenderer2D* renderer, const HyVec3* pos, const HyVec2* size, uint32_t textureID);
static void DrawQuad2T(HyRenderer2D* renderer, const HyVec2* pos, const HyVec2* size, uint32_t textureID);

static HyRenderer2DStats HyRenderer2D_GetStats(HyRenderer2D* renderer)
{
    return renderer->stats;
}

static void HyRenderer2D_ResetStats(HyRenderer2D* renderer)
{
    memset(&renderer->stats, 0, sizeof(HyRenderer2DStats));
}

static void HyRenderer2D_Init(HyRenderer2D* renderer)
{
    HY_ASSERT(!renderer->quadVertexBufferBase, "Called HyRenderer2D_Init more than once.");
    
    renderer->maxQuadCount = 10000; // Actaully it crashes
    renderer->maxVertexCount = renderer->maxQuadCount * 4;
    renderer->maxIndexCount = renderer->maxQuadCount * 6;
    renderer->maxTextures = 32; // TODO Query driver for max texture slots
    
    renderer->quadVertexBufferBase = (HyQuadVertex *)malloc(sizeof(HyQuadVertex) * renderer->maxVertexCount);
    
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
    
    uint32_t* indices = (uint32_t *)malloc(sizeof(uint32_t) * renderer->maxIndexCount);
    uint32_t offset = 0;
    
    for (uint32_t i = 0; i < renderer->maxIndexCount; i += 6)
    {
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
    
    free(indices);
    
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
    
    renderer->textureSlots[renderer->whiteTextureSlot] = renderer->whiteTexture;
    for (size_t i = 1; i < renderer->maxTextures; ++i)
    {
        renderer->textureSlots[i] = 0;
    }
    
    HyRenderer2D_ResetStats(renderer);
}

static void HyRenderer2D_Shutdown(HyRenderer2D* renderer)
{
    GL_CALL(glDeleteVertexArrays(1, &renderer->vao));
    GL_CALL(glDeleteBuffers (1, &renderer->vbo));
    GL_CALL(glDeleteBuffers (1, &renderer->ebo));
    
    free(renderer->quadVertexBufferBase);
    
    GL_CALL(glDeleteTextures(1, &renderer->whiteTexture));
}

static void HyRenderer2D_BeginScene(HyRenderer2D* renderer, HyCamera2D* camera)
{
    // TODO(alex): Camera default to NULL;
    if (camera)
        renderer->camera = camera;
    
    HY_Shader_Bind(renderer->textureShader);
    HY_Shader_SetMat4(renderer->textureShader, "u_View", &renderer->camera->viewMatrix);
    HY_Shader_SetMat4(renderer->textureShader, "u_Projection", &renderer->camera->projectionMatrix);
    
    renderer->quadIndexCount = 0;
    renderer->quadVertexBufferPtr = renderer->quadVertexBufferBase;
    renderer->textureSlotIndex = 1;
    
    int loc = glGetUniformLocation(renderer->textureShader->id, "u_Textures");
    int samplers[32];
    for (int i = 0; i < 32; ++i)
    {
        samplers[i] = i; // TODO(alex): zero?
    }
    GL_CALL(glUniform1iv(loc, 32, samplers));
}

static void HyRenderer2D_EndScene(HyRenderer2D* renderer)
{
    uint32_t size = (uint32_t)((uint8_t *)renderer->quadVertexBufferPtr - (uint8_t *)renderer->quadVertexBufferBase);
    
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo));
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, size, renderer->quadVertexBufferBase));
    
    HyRenderer2D_Flush(renderer);
}

static void HyRenderer2D_Flush(HyRenderer2D* renderer)
{
    for (uint32_t i = 0; i < renderer->textureSlotIndex; ++i)
    {
        glBindTextureUnit(i, renderer->textureSlots[i]);
    }
    
    GL_CALL(glBindVertexArray(renderer->vao));
    GL_CALL(glDrawElements(GL_TRIANGLES, renderer->quadIndexCount, GL_UNSIGNED_INT, NULL));
    renderer->stats.drawCount++;
}

static void DrawQuad(HyRenderer2D* renderer, const HyVec3* pos, const HyVec2* size, uint32_t textureID, HyVec4* color)
{
    // Checks if we have room in our current batch for more quads.
    // 31 because the first one is a 1x1 white texture
    if (renderer->quadIndexCount >= renderer->maxIndexCount || renderer->textureSlotIndex > 31)
    {
    	HyRenderer2D_EndScene(renderer);
    	HyRenderer2D_BeginScene(renderer, NULL);
    }
    
    float textureIndex = 0.0f;
    // Skip first 1x1 white texture
    // hHecks if current texture was used to render another quad.
    for (uint32_t i = 1; i < renderer->textureSlotIndex; ++i)
    {
    	if (renderer->textureSlots[i] == textureID)
    	{
    		textureIndex = (float)i;
    		break;
    	}
    }
    
    // Didn't found in previous loop. Put in next available free texture slot.
    if (textureIndex == 0.0f)
    {
    	textureIndex = (float)renderer->textureSlotIndex;
    	renderer->textureSlots[renderer->textureSlotIndex] = textureID;
    	renderer->textureSlotIndex++;
    }
    
    renderer->quadVertexBufferPtr->Pos = (HyVec3){ pos->x, pos->y, pos->z };
    renderer->quadVertexBufferPtr->Color = *color;
    renderer->quadVertexBufferPtr->TexCoord = (HyVec2){ 0.0f, 0.0f };
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadVertexBufferPtr->Pos = (HyVec3){ pos->x + size->x, pos->y, pos->z };
    renderer->quadVertexBufferPtr->Color = *color;
    renderer->quadVertexBufferPtr->TexCoord = (HyVec2){ 1.0f, 0.0f };
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadVertexBufferPtr->Pos = (HyVec3){ pos->x + size->x, pos->y + size->y, pos->z };
    renderer->quadVertexBufferPtr->Color = *color;
    renderer->quadVertexBufferPtr->TexCoord =  (HyVec2){ 1.0f, 1.0f };
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadVertexBufferPtr->Pos = (HyVec3){ pos->x, pos->y + size->y, pos->z };
    renderer->quadVertexBufferPtr->Color = *color;
    renderer->quadVertexBufferPtr->TexCoord = (HyVec2){ 0.0f, 1.0f };
    renderer->quadVertexBufferPtr->TexIndex = textureIndex;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadIndexCount += 6;
    renderer->stats.quadCount++;
}

static void DrawQuad3C(HyRenderer2D* renderer, const HyVec3* pos, const HyVec2* size, const HyVec4* color)
{
    // Checks if we have room in our current batch for more quads.
    // 31 because the first one is a 1x1 white texture
    if (renderer->quadIndexCount >= renderer->maxIndexCount)
    {
    	HyRenderer2D_EndScene(renderer);
    	HyRenderer2D_BeginScene(renderer, NULL);
    }
    
    renderer->quadVertexBufferPtr->Pos = (HyVec3){ pos->x, pos->y, pos->z };
    renderer->quadVertexBufferPtr->Color = *color;
    renderer->quadVertexBufferPtr->TexCoord = (HyVec2){ 0.0f, 0.0f };
    renderer->quadVertexBufferPtr->TexIndex = (float)renderer->whiteTextureSlot;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadVertexBufferPtr->Pos = (HyVec3){ pos->x + size->x, pos->y, pos->z };
    renderer->quadVertexBufferPtr->Color = *color;
    renderer->quadVertexBufferPtr->TexCoord = (HyVec2){ 1.0f, 0.0f };
    renderer->quadVertexBufferPtr->TexIndex = (float)renderer->whiteTextureSlot;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadVertexBufferPtr->Pos = (HyVec3){ pos->x + size->x, pos->y + size->y, pos->z };
    renderer->quadVertexBufferPtr->Color = *color;
    renderer->quadVertexBufferPtr->TexCoord = (HyVec2){ 1.0f, 1.0f };
    renderer->quadVertexBufferPtr->TexIndex = (float)renderer->whiteTextureSlot;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadVertexBufferPtr->Pos = (HyVec3){ pos->x, pos->y + size->y, pos->z };
    renderer->quadVertexBufferPtr->Color = *color;
    renderer->quadVertexBufferPtr->TexCoord = (HyVec2){ 0.0f, 1.0f };
    renderer->quadVertexBufferPtr->TexIndex = (float)renderer->whiteTextureSlot;
    renderer->quadVertexBufferPtr++;
    
    renderer->quadIndexCount += 6;
    renderer->stats.quadCount++;
}

static void DrawQuad2T(HyRenderer2D* renderer, const HyVec2* pos, const HyVec2* size, uint32_t textureID)
{
    HyVec3 temp = { pos->x, pos->y, 1.0f };
    //DrawQuad3T(renderer, &temp, size, textureID);
}

static void DrawQuad2C(HyRenderer2D* renderer, const HyVec2* pos, const HyVec2* size, const HyVec4* color)
{
    HyVec3 tempPos = { pos->x, pos->y, 1.0f };
    DrawQuad3C(renderer, &tempPos, size, color);
}

//~ Mesh

#if 0

typedef struct
{
    HyVec3 position;
    HyVec3 normal;
    HyVec2 texCoords;
} HyVertex;

typedef struct
{
    HyVertex* vertices;
    uint32_t* indices;
    HyTexture* textures;
    
    uint32_t vertexCount;
    uint32_t indexCount;
    uint32_t textureCount;
    
    uint32_t VAO, VBO, EBO;
} HyMesh;

struct HyModel
{
    HyMesh* meshes;
    uint32_t meshCount;
};

static HyMesh HyMesh_Create(HyVertex* pVertices, uint32_t vertexCount,
                            uint32_t* pIndices, uint32_t indexCount,
                            HyTexture* pTextures, uint32_t textureCount)
{
    HyMesh mesh;
    
    mesh.vertices = pVertices;
    mesh.vertexCount = vertexCount;
    mesh.indices = pIndices;
    mesh.indexCount = indexCount;
    mesh.textures = pTextures;
    mesh.textureCount = textureCount;
    
    // TODO(alex): use our own vertex array
    
    GL_CALL(glGenVertexArrays(1, &mesh.VAO));
    GL_CALL(glBindVertexArray(mesh.VAO));
    
    GL_CALL(glGenBuffers(1, &mesh.VBO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(HyVertex), pVertices, GL_STATIC_DRAW));
    
    GL_CALL(glGenBuffers(1, &mesh.EBO));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), pIndices, GL_STATIC_DRAW));
    
    // vertex positions
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HyVertex), (void*)offsetof(HyVertex, position)));
    // vertex normals
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(HyVertex), (void*)offsetof(HyVertex, normal)));
    // vertex texture coords
    GL_CALL(glEnableVertexAttribArray(2));
    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(HyVertex), (void*)offsetof(HyVertex, texCoords)));
    
    GL_CALL(glBindVertexArray(0));
    
    return mesh;
}


static HyModel HY_Model_Create(const char* path);
static void HY_Model_ProcessNode(aiNode* node, aiScene* scene);
static HyMesh HY_Model_ProcessMesh(aiMesh* mesh, aiScene* scene);

static HyModel HY_Model_Create(const char* path)
{
    HyModel hyModel = {0};
    
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        HY_CORE_ASSERT(false, "Can't load model");
        return;
    }
    
    hyModel.directory = ".\assets";
    
    return hyModel;
}

static void HY_Model_ProcessNode(HyModel* pModel, aiNode* node, aiScene* scene)
{
    // process all the node's meshes (if any)
    for(uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

static HyMesh HY_Model_ProcessMesh(aiMesh* mesh, aiScene* scene)
{
    HyVertex* vertices;
    uint32_t* indices;
    HyTexture* textures;
    
    for(uint32_t i = 0; i < mesh->mNumVertices; ++i)
    {
        HyVertex vertex;
        // process vertex positions, normals and texture coordinates
        vertex.position = HyVec3 {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };
        vertex.normals = HyVec3 {
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z
        };
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            HyVec2 vec;
            vertex.texCoords = HyVec2 {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }
        else
        {
            vertex.texCoords = HyVec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    
    // process indices
    for(uint32_t i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for(uint32_t j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    } 
    
    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    
    return Mesh(vertices, indices, textures);
}

//~ Renderer

typedef struct
{
    HyCamera* camera;
} HyRenderer;

static void HyRenderer_Init(HyRenderer* renderer)
{
    // TODO(alex): Check if error callback is supported
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(glDebugOutput, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

static void HY_Renderer_BeginScene(HyRenderer* renderer, HyCamera* camera = NULL)
{
    if (camera)
    {
        renderer->camera = camera;
    }
    
    //HY_ASSERT(renderer->camera, "Renderer camera non passed on BeginScene()");
}

static void HY_Renderer_SubmitMesh(HyRenderer* pRenderer, HyMesh* pMesh, HyShader* pShader, glm::mat4* pModel)
{
    HY_Shader_Bind(pShader);
    HY_Shader_SetMat4(pShader, "u_Model", pModel); // TODO(alex): use ecs
    HY_Shader_SetMat4(pShader, "u_View", &pRenderer->camera->viewMatrix);
    HY_Shader_SetMat4(pShader, "u_Projection", &pRenderer->camera->projectionMatrix);
    
    // TODO(alex): Use correct mesh textures
    
    GL_CALL(glBindVertexArray(pMesh->VAO));
    GL_CALL(glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0));
    //GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
    GL_CALL(glBindVertexArray(0));
}

static void HY_Renderer_SubmitModel(HyModel* pModel, HyShader* pShader)
{
    for (uint32_t i = 0; i < pModel->meshCount; ++i)
    {
        HY_Renderer_SubmitMesh(pModel->meshes[i], pShader);
    }
}

static void HY_Renderer_Submit(HyRenderer* renderer, HyShader* shader)
{
    HY_Shader_Bind(shader);
    HY_Shader_SetMat4(shader, "u_View", &renderer->camera->viewMatrix);
    HY_Shader_SetMat4(shader, "u_Projection", &renderer->camera->projectionMatrix);
}

static void HY_Renderer_Submit(HyRenderer* renderer, HyShader* shader, HyVertexArray* vertexArray)
{
    HY_PROFILE_FUNCTION();
    
    HY_Shader_Bind(shader);
    HY_VertexArray_Bind(vertexArray);
    
    HY_Shader_SetMat4(shader, "u_View", &renderer->camera->viewMatrix);
    HY_Shader_SetMat4(shader, "u_Projection", &renderer->camera->projectionMatrix);
    
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, vertexArray->vertexBuffers[0]->size));
}

static void HY_Renderer_EndScene()
{
}
#endif

static void HY_SetClearColorCmd(HyColor* color)
{
    GL_CALL(glClearColor(color->r, color->g, color->b, color->a));
}

static void HY_SetClearColorCmdByColors(float r, float g, float b, float a)
{
    GL_CALL(glClearColor(r, g, b, a));
}

static void HY_ClearCmd()
{
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

static void HY_ClearColorCmd()
{
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}
