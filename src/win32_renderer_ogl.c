#include "hy_renderer.h"

//~
// OpenGL Error handling
//

#if HY_SLOW

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

#else
#define ASSERT(x)
#define GL_CALL(x) x;
#endif // HY_SLOW

//~
// Textures
//

struct HyTexture {
    uint32_t    rendererID;
    uint32_t    width;
    uint32_t    height;
    uint32_t    channels;
    const char* path;
};

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

//~
// Framebuffer
//

struct HyFramebuffer {
    uint32_t rendererID;
    uint32_t colorAttachmentRendererID;
    uint32_t depthAttachmentRendererID;
    
    uint32_t width;
    uint32_t height;
};

// TODO(alex): Make it return a pointer;
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
// Shader
//

struct HyShader {
    char name[128]; // TODO(alex): malloc
    uint32_t    id;
    
    int uniformLocationsMap[HY_SHADER_MAP_SIZE];
};

struct HyShaderLibrary {
    int todo;
    // TODO(alex): Implement
};

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

//~
// Camera2D
//

/// Camera 2D used for rendering.
struct HyCamera2D {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 viewProjectionMatrix;
    
    float width;
    float height;
    float nearPlane;
    float farPlane;
    
    vec4 viewport;
};

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

//~
// Renderer2D
//

typedef struct {
    vec3  Pos;
    float TexIndex;
    HyColor Color;
    vec2  TexCoord;
} HyQuadVertex;

/// Batch 2D Renderer stats. Start/reset must be managed manually.
struct HyRenderer2DStats {
    uint32_t drawCount;
    uint32_t quadCount;
};

/// Batch 2D Renderer.
struct HyRenderer2D {
    HyCamera2D* camera;
    
    uint32_t maxQuadCount;
    uint32_t maxVertexCount;
    uint32_t maxIndexCount;
    uint32_t maxTextures;
    
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
    
    HyShader* textureShader;
    uint32_t  whiteTexture;
    uint32_t  whiteTextureSlot;
    
    uint32_t quadIndexCount;
    
    HyQuadVertex* quadVertexBufferBase;
    HyQuadVertex* quadVertexBufferPtr;
    
    uint32_t textureSlots[32]; // TODO(alex): Maybe malloc ?
    uint32_t textureSlotIndex;
    
    HyTexture* asciiTexture; // Debug text
    
    HyRenderer2DStats stats;
};

global_variable HyRenderer2D g_renderer;

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
    
#if HY_SLOW
    GL_CALL(glEnable(GL_DEBUG_OUTPUT));
    GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
    GL_CALL(glDebugMessageCallback(hy_gl_debug_output, NULL));
    GL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE));
#endif
    
    renderer->maxQuadCount = 10000;
    renderer->maxVertexCount = renderer->maxQuadCount * 4;
    renderer->maxIndexCount = renderer->maxQuadCount * 6;
    GL_CALL(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &(int32_t)renderer->maxTextures));
    
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
    
    // Set white texture for rendering quads/textures in a single draw call.
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
    // TODO(alex): Maybe use a frame allocator?
    uint32_t* samplers = hy_malloc(sizeof(uint32_t) * renderer->maxTextures);
    for (int i = 0; i < renderer->maxTextures; ++i) {
        samplers[i] = i; // TODO(alex): zero?
    }
    GL_CALL(glUniform1iv(loc, renderer->maxTextures, (int32_t*)samplers));
    
    hy_free(samplers);
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
        // TODO(alex): What is the difference between the separate calls and the single one?
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
