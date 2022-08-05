#include "RAIOpenGL33.h"
#include "Compat/OpenGL.h"
#include "FileSystem/Extras.h"


#define GL_PROCS \
X(Clear,                   void, gl::Enum mask) \
X(Flush,                   void, void) \
X(Finish,                  void, void) \
X(ClearColor,              void, float r, float g, float b, float a) \
X(Enable,                  void, gl::Enum cap) \
X(Disable,                 void, gl::Enum cap) \
X(GenBuffers,              void, gl::Sizei n, gl::UInt *buffers) \
X(BindBuffer,              void, gl::Enum target, gl::UInt buffer) \
X(BindBufferBase,          void, GLenum target, GLuint index, GLuint buffer) \
X(BindFramebuffer,         void, gl::Enum target, gl::UInt framebuffer) \
X(Viewport,                void, gl::Int x, gl::Int y, gl::Sizei width, gl::Sizei height) \
X(GenVertexArrays,         void, gl::Sizei n, gl::UInt *arrays) \
X(BindVertexArray,         void, gl::UInt array) \
X(VertexAttribPointer,     void, gl::UInt index, gl::Int size, gl::Enum type, gl::Bool normalized, gl::Sizei stride, const void *pointer) \
X(EnableVertexAttribArray, void, gl::UInt array) \
X(BufferData,              void, gl::Enum target, gl::Sizei size, const void *data, GLenum usage) \
X(GetError,                gl::Enum, void) \
X(CreateShader,            gl::UInt, gl::Enum type) \
X(ShaderSource,            void, gl::UInt shader, gl::Sizei count, const gl::Char *const* string, const gl::Int *length) \
X(CompileShader,           void, gl::UInt shader) \
X(GetShaderiv,             void, gl::UInt shader, gl::Enum pname, gl::Int *params) \
X(GetShaderInfoLog,        void, gl::UInt shader, gl::Sizei bufSize, gl::Sizei *length, gl::Char *infoLog) \
X(CreateProgram,           gl::UInt, void) \
X(AttachShader,            void, gl::UInt program, gl::UInt shader) \
X(LinkProgram,             void, gl::UInt program) \
X(UseProgram,              void, gl::UInt program) \
X(GetProgramiv,            void, gl::UInt program, gl::Enum pname, gl::Int *params) \
X(GetProgramInfoLog,       void, gl::UInt program, gl::Sizei bufSize, gl::Sizei *length, gl::Char *infoLog) \
X(GetUniformBlockIndex,    gl::UInt, gl::UInt program, const gl::Char *uniformBlockName) \
X(UniformBlockBinding,     void, gl::UInt program, gl::UInt uniformBlockIndex, gl::UInt uniformBlockBinding) \
X(DrawArrays,              void, gl::Enum mode, gl::Int first, gl::Sizei count) \
X(DeleteVertexArrays,      void, gl::Sizei n, const gl::UInt *arrays) \
X(IsVertexArray,           gl::Bool, gl::UInt array) \
X(MapBuffer,               void*, gl::Enum target, gl::Enum access) \
X(UnmapBuffer,             void, gl::Enum target) \
X(GenTextures,             void, gl::Sizei n, gl::UInt *textures) \
X(BindTexture,             void, gl::Enum target, gl::UInt texture) \
X(TexImage2D,              void, gl::Enum target, gl::Int level, gl::Int internalformat, gl::Sizei width, gl::Sizei height, gl::Int border, gl::Enum format, gl::Enum type, const void *pixels) \
X(ActiveTexture,           void, gl::Enum target) \
X(TexParameteri,           void, gl::Enum target, gl::Enum pname, gl::Int param) \
X(PixelStorei,             void, gl::Enum pname, gl::Int param) \
X(VertexAttribDivisor,     void, gl::UInt index, gl::UInt divisor) \


#define GLC(proc) \
    do {                                                               \
        GL.proc;                                                       \
        int err = GL.GetError();                                       \
        if (err != 0) {                                                \
            print(LIT("Unexpected gl error: $ on call $\n  -> $:$\n"), \
                TFmtHex(err),                                          \
                LIT(#proc),                                            \
                LIT(__FILE__),                                         \
                __LINE__);                                             \
            print_backtrace();                                         \
        }                                                              \
    } while (0)

#define GLCR(proc) \
    do {                                                               \
        proc;                                                          \
        int err = GL.GetError();                                       \
        if (err != 0) {                                                \
            print(LIT("Unexpected gl error: $ on call $\n  -> $:$\n"), \
                TFmtHex(err),                                          \
                LIT(#proc),                                            \
                LIT(__FILE__),                                         \
                __LINE__);                                             \
        }                                                              \
    } while (0)



// Typedefs
#define X(name, rt, ...) typedef rt (MCONCAT(ProcGL,name))(__VA_ARGS__);
GL_PROCS
#undef X

struct GL33AttributeBinding {
    gl::UInt id = 0;
    gl::Int size;
    gl::Enum type;
    gl::Bool normalized;
    gl::Sizei stride;
    u64 offset;
};

struct RAICache {
    enum {
        NumLayoutAttrs = 5,
        NumBufferSlots = 4,
    };
    InputLayoutAttr attrs[NumLayoutAttrs] = {};
    u32             num_attrs = 0;
    BufferRes       buffers[NumBufferSlots] = {};
    u32             num_buffers = 0;
    bool changed_attrs = false;
};

static struct {
#define X(name, rt, ...) MCONCAT(ProcGL,name) *name = 0;
GL_PROCS
#undef X

    // @todo: Shared contexts don't share vaos
    gl::UInt vao;

    gl::UInt current_constant_buffer;
    gl::UInt current_shader_program;
    gl::Enum current_topology;

    RAICache cached = {};

    GL33AttributeBinding attributes[16] = {};
} GL;


static void rai_gl33_update_layout(Slice<InputLayoutAttr> new_attrs) {
    GL.cached.num_attrs = new_attrs.count;
    for (u32 i = 0; i < new_attrs.count; ++i) {
        if (new_attrs[i] != GL.cached.attrs[i]) {
            GL.cached.attrs[i] = new_attrs[i];
            GL.cached.changed_attrs = true;
        }
    }
}

static void rai_gl33_update_bindings(const Bindings &bindings) {
    GL.cached.num_buffers = bindings.buffers.count;
    for (u32 i = 0; i < bindings.buffers.count; ++i) {
        if (GL.cached.buffers[i].handle != bindings.buffers[i]->handle) {
            GL.cached.buffers[i] = *bindings.buffers[i];
            GL.cached.changed_attrs = true;
        }
    }
}

static void rai_gl33_check_cache() {

    if (!GL.cached.changed_attrs) return;

    GL.cached.changed_attrs = false;
    u32 stride = 0;

    for (u32 i = 0; i < RAICache::NumLayoutAttrs; ++i) {
        gl::Int size;
        gl::Enum type;
        gl::Sizei istride;
        input_layout_attr_to_glprops(
            GL.cached.attrs[i].kind,
            size,
            type,
            istride);
        stride += istride;
    }

    if (!GL.IsVertexArray(GL.vao)) {
        GLC(GenVertexArrays(1, &GL.vao));
        GLC(BindVertexArray(GL.vao));
    }

    for (u32 i = 0; i < GL.cached.num_attrs; ++i) {
        InputLayoutAttr &attr = GL.cached.attrs[i];

        GLC(BindBuffer(
            gl::ArrayBuffer,
            (gl::UInt)GL.cached.buffers[attr.slot].handle));

        gl::Int size;
        gl::Enum type;
        gl::Sizei istride;
        input_layout_attr_to_glprops(
            attr.kind,
            size,
            type,
            istride);
        GLC(VertexAttribPointer(
            i,
            size,
            type,
            false,
            attr.stride,
            (void*)attr.offset));
        GLC(EnableVertexAttribArray(i));

        if (attr.usage == InputLayoutAttrUsage::PerInstance) {
            GLC(VertexAttribDivisor(i, attr.instance_stride));
        } else {
            GLC(VertexAttribDivisor(i, 0));
        }
    }

}

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
        buffer_id,
        desc->kind
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
    RenderPass result = {};
    // the default one
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

PROC_RAI_CREATE_SHADER_PART(rai_gl33_create_shader_part) {
    gl::Enum ekind = shader_part_kind_to_glenum(kind);

    if (!source) {
        return Err(RAIError::FailedToAllocateResource);
    }

    gl::UInt shader_id = GL.CreateShader(ekind);
    int err = GL.GetError();
    if (err != 0) {
        return Err(RAIError::FailedToAllocateResource);
    }

    gl::Int source_size = (gl::Int)source.size;
    const gl::Char *source_str = (char*)source.buffer;
    GL.ShaderSource(shader_id, 1, &source_str, &source_size);

    GL.CompileShader(shader_id);

    gl::Int success;
    GL.GetShaderiv(shader_id, gl::CompileStatus, &success);

    if (!success) {
        char info_log[512];

        GL.GetShaderInfoLog(shader_id, sizeof(info_log), 0, info_log);
        print(LIT("GL: Shader compilation failed -- $\n"), Str(info_log));

        return Err(RAIError::FailedToAllocateResource);
    }

    ShaderPartRes result = {};
    result.kind = kind;
    result.handle = (u64)shader_id;
    return Ok(result);
}

PROC_RAI_CREATE_SHADER(rai_gl33_create_shader) {

    gl::UInt program = GL.CreateProgram();

    GL.AttachShader(program, (gl::UInt)desc->vertex->handle);
    GL.AttachShader(program, (gl::UInt)desc->pixel->handle);

    GL.LinkProgram(program);

    gl::Int success;
    GL.GetProgramiv(program, gl::LinkStatus, &success);

    if (!success) {
        char info_log[512];
        GL.GetProgramInfoLog(program, sizeof(info_log), 0, info_log);
        print(LIT("GL: program linkage failed -- $\n"), Str(info_log));

        return Err(RAIError::FailedToAllocateResource);
    }

    return Ok(ShaderRes{(u64)program});
}

PROC_RAI_SET_BINDINGS(rai_gl33_set_bindings) {

    rai_gl33_update_bindings(*bindings);

    gl::UInt index_buffer_id = bindings->index
        ? (gl::UInt)bindings->index->handle
        : 0;
    gl::UInt const_buffer_id = bindings->constant
        ? (gl::UInt)bindings->constant->handle
        : 0;

    GLC(BindBuffer(gl::ElementArrayBuffer, index_buffer_id));
    GLC(BindBufferBase(gl::UniformBuffer, 0, const_buffer_id));

    for (int i = 0; i < bindings->ps_textures.count; ++i) {
        TextureRes *r = bindings->ps_textures[i];
        GLC(ActiveTexture(gl::Texture0 + i));
        GLC(BindTexture(gl::Texture2D, (gl::UInt)r->handle));
    }
}

PROC_RAI_SET_PIPELINE(rai_gl33_set_pipeline) {
    auto program = (gl::UInt)pipeline->shader->handle;

    // @todo don't generate vaos every time

    GLC(UseProgram(program));

    rai_gl33_update_layout(pipeline->layout.attrs);

    GL.current_topology = topology_to_gl33_topology(pipeline->topology);
    return true;
}

PROC_RAI_DRAW(rai_gl33_draw) {

    rai_gl33_check_cache();

    // @todo: num_instances?
    GLC(DrawArrays(GL.current_topology, base, num_elements));
}

PROC_RAI_GET_DEFAULT_RENDER_PASS(rai_gl33_get_default_render_pass) {
    return RenderPass {0};
}

PROC_RAI_MAP_BUFFER(rai_gl33_map_buffer) {
    gl::Enum buffer_kind = buffer_res_kind_to_glenum(buffer->kind);
    GLC(BindBuffer(buffer_kind, (gl::UInt)buffer->handle));
    void *ptr;
    GLCR(ptr = GL.MapBuffer(buffer_kind, gl::ReadWrite));

    return ptr;
}

PROC_RAI_UNMAP_BUFFER(rai_gl33_unmap_buffer) {
    gl::Enum buffer_kind = buffer_res_kind_to_glenum(buffer->kind);
    GLC(UnmapBuffer(buffer_kind));
}

PROC_RAI_CREATE_TEXTURE(rai_gl33_create_texture) {
    gl::UInt texture;
    GLC(GenTextures(1, &texture));
    GLC(BindTexture(gl::Texture2D, texture));

    gl::Enum format, type;
    pixel_format_to_gl_format(desc->format, format, type);
    GLC(PixelStorei(gl::UnpackAlignment, 1));
    GLC(TexParameteri(gl::Texture2D, gl::TextureMinFilter, gl::Linear));
    GLC(TexParameteri(gl::Texture2D, gl::TextureMagFilter, gl::Linear));

    GL.TexImage2D(
        gl::Texture2D,
        0,
        gl::Rgba,
        desc->size.x,
        desc->size.y,
        0,
        format,
        type,
        desc->data.buffer);

    auto err = GL.GetError();
    if (err != 0) {
        return Err(RAIError::FailedToAllocateResource);
    }

    return Ok(TextureRes {
        (u64)texture
    });
}

PROC_RAI_INITIALIZE(rai_gl33_init) {
    auto *params = (RAIGL33InitParams*)vparams;
    Str failed_proc = load_procs(params->get_proc);

    if (failed_proc != Str::NullStr) {
        print(LIT("Failed to load proc $"), failed_proc);
        return false;
    }

    for (u32 i = 0; i < RAICache::NumLayoutAttrs; ++i) {
        GL.cached.attrs[i].name = 0;
    }

    result.create_buffer = rai_gl33_create_buffer;
    result.create_texture = rai_gl33_create_texture;
    result.create_shader_part = rai_gl33_create_shader_part;
    result.create_shader = rai_gl33_create_shader;
    result.get_default_render_pass = rai_gl33_get_default_render_pass;
    result.set_render_pass = rai_gl33_set_render_pass;
    result.set_pipeline = rai_gl33_set_pipeline;
    result.set_bindings = rai_gl33_set_bindings;
    result.map_buffer = rai_gl33_map_buffer;
    result.unmap_buffer = rai_gl33_unmap_buffer;
    result.draw = rai_gl33_draw;
    return true;
}