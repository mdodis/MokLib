#include "RAIOpenGL33.h"
#include "Compat/OpenGL.h"
#include "FileSystem/Extras.h"

gl::Enum buffer_res_kind_to_glenum(EBufferResKind kind) {
    static gl::Enum table[3] = {
        gl::ArrayBuffer,        // Vertex
        gl::ElementArrayBuffer, // Index
        gl::UniformBuffer,      // Constant
    };
    return table[kind];
}

#define GL_PROCS \
X(Clear,                   void, gl::Enum mask) \
X(ClearColor,              void, float r, float g, float b, float a) \
X(GenBuffers,              void, gl::Sizei n, gl::UInt *buffers) \
X(BindBuffer,              void, gl::Enum target, gl::UInt buffer) \
X(BindFramebuffer,         void, gl::Enum target, gl::UInt framebuffer) \
X(Viewport,                void, gl::Int x, gl::Int y, gl::Sizei width, gl::Sizei height) \
X(GenVertexArrays,         void, gl::Sizei n, gl::UInt *arrays) \
X(BindVertexArray,         void, gl::UInt array) \
X(VertexAttribPointer,     void, gl::UInt index, gl::Int size, gl::Enum type, gl::Bool normalized, gl::Sizei stride, const void *pointer) \
X(EnableVertexAttribArray, void, gl::UInt array) \
X(BufferData,              void, gl::Enum target, gl::Sizei size, const void *data, GLenum usage) \
X(GetError,                gl::Enum, void) \


#define GLC(proc) \
    do {                                                          \
        GL.proc;                                                  \
        int err = GL.GetError();                                  \
        if (err != 0) {                                           \
            print(LIT("Unexpected gl error: $ on call $ At $:$"), \
                err,                                              \
                LIT(#proc),                                       \
                LIT(__FILE__),                                    \
                __LINE__);                                        \
        }                                                         \
    } while (0)


// Typedefs
#define X(name, rt, ...) typedef rt (MCONCAT(ProcGL,name))(__VA_ARGS__);
GL_PROCS
#undef X

static struct {
#define X(name, rt, ...) MCONCAT(ProcGL,name) *name = 0;
GL_PROCS
#undef X

    // @todo: Shared contexts don't share vaos
    gl::UInt vao;
} GL;

static Str load_procs(ProcRAIGLGetProcAddress *get_proc_address) {
    #define X(name, rt, ...) \
        *((void**)&(GL.name)) = get_proc_address(MSTR(MCONCAT(gl,name))); \
        if (GL.name == 0) return LIT(#name);
    GL_PROCS
    #undef X

    return Str::NullStr;
}

PROC_RAI_CREATE_BUFFER(rai_gl33_create_buffer) {
    gl::UInt buffer_id;
    gl::Enum buffer_kind = buffer_res_kind_to_glenum(desc->kind);

    GLC(GenBuffers(1, &buffer_id));
    GLC(BindBuffer(buffer_kind, buffer_id));

    GL.BufferData(buffer_kind, desc->size, desc->ptr, gl::StaticDraw);
    int err = GL.GetError();
    if (err != 0) {
        return Err(RAIError::FailedToAllocateResource);
    }

    GLC(BindBuffer(buffer_kind, 0));

    BufferRes result = {
        buffer_id
    };

    return Ok(result);
}

PROC_RAI_GET_DEFAULT_TEXTURE(rai_gl33_get_default_texture) {
    switch (kind) {
        case TextureResUsage::RenderTarget:
            return TextureRes {0};
        case TextureResUsage::DepthStencil:
            return TextureRes {0};
        default:
            return TextureRes {0};
    }
}

PROC_RAI_CREATE_RENDER_PASS(rai_gl33_create_render_pass) {
    // the default one
    RenderPass result = {};
    if (desc->color_attachment->handle == 0) {
        result.handle = 0;
    }

    return result;
}

PROC_RAI_SET_RENDER_PASS(rai_gl33_set_render_pass) {
    auto framebuffer_id = *((gl::UInt*)&pass->handle);
    GL.BindFramebuffer(gl::Framebuffer, framebuffer_id);
    GL.Viewport(0, 0, size.x, size.y);
    GL.ClearColor(0,0,0,1);
    GL.Clear(gl::ColorBufferBit | gl::DepthBufferBit);
}

PROC_RAI_INITIALIZE(rai_gl33_init) {
    auto *params = (RAIGL33InitParams*)vparams;
    Str failed_proc = load_procs(params->get_proc);

    if (failed_proc != Str::NullStr) {
        print(LIT("Failed to load proc $"), failed_proc);
        return false;
    }

    GL.GenVertexArrays(1, &GL.vao);

    result.create_buffer = rai_gl33_create_buffer;
    result.set_render_pass = rai_gl33_set_render_pass;
    return true;
}