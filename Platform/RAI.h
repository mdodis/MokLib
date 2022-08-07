#pragma once
#include "Base.h"
#include "Importers/Image/Image.h"
#include "Importers/Model/Model.h"
#include "Result.h"
#include "Containers/Slice.h"
/**
 * @todo: Add constant buffer bind slot in set_bindings
 */

namespace BufferResKind {
    enum Type : u32 {
        Vertex,
        Index,
        Constant
    };
}
typedef BufferResKind::Type EBufferResKind;

struct BufferRes {
    u64 handle;
    EBufferResKind kind;
};

struct BufferResDesc {
    EBufferResKind kind;
    u32            size;
    void          *ptr;
};

namespace ShaderPartResKind {
    enum Type : u32 {
        Vertex,
        Pixel
    };
}
typedef ShaderPartResKind::Type EShaderPartResKind;

struct ShaderPartRes {
    EShaderPartResKind kind;
    u64                handle;
};

struct ShaderRes {
    u64 handle;
};

struct ShaderResDesc {
    ShaderPartRes *vertex;
    ShaderPartRes *pixel;
};

namespace TextureResUsage {
    enum Type : u32 {
        Default,        // Used as a resource for shaders
        DepthStencil,   // Used as the target for depth stencil output
        RenderTarget    // Used as a target for rendering results
    };
};
typedef TextureResUsage::Type ETextureResUsage;

namespace TextureResFormat {
    enum Type : u32 {
        Float4, // 128 bit RGBA pixel, 32 bits per component
        Float   // 32 bit floating point pixel size
    };
}
typedef TextureResFormat::Type ETextureResFormat;

struct TextureRes {
    u64 handle;
};

struct TextureResDesc {
    Vec2i             size;
    ETextureResUsage  usage;
    PixelFormat::Type format;
    Raw               data;
    u32               pitch;
};

namespace InputLayoutAttrKind {
    enum Type : u32 {
        None,
        Float2,
        Float3,
        Float4,
    };
};
typedef InputLayoutAttrKind::Type EInputLayoutAttrKind;

namespace InputLayoutAttrUsage {
    enum Type : u32 {
        PerVertex,
        PerInstance
    };
};
typedef InputLayoutAttrUsage::Type EInputLayoutAttrUsage;

struct InputLayoutAttr {
    u32                   slot = 0;
    const char            *name = 0;
    EInputLayoutAttrKind  kind = InputLayoutAttrKind::None;
    u32                   offset = 0;
    u32                   stride = 0;
    EInputLayoutAttrUsage usage = InputLayoutAttrUsage::PerVertex;
    u32                   instance_stride = 0;
};

static _inline bool operator==(const InputLayoutAttr &a, const InputLayoutAttr &b) {
    return
        (a.slot == b.slot) &&
        ((a.name == b.name) || ((a.name != 0) && (b.name != 0) && (strcmp(a.name, b.name) == 0))) &&
        (a.kind == b.kind) &&
        (a.offset == b.offset) &&
        (a.stride == b.stride) &&
        (a.usage == b.usage) &&
        (a.instance_stride == b.instance_stride);
}

static _inline bool operator!=(const InputLayoutAttr &a, const InputLayoutAttr &b) {
    return !(a == b);
}

struct InputLayout {
    Slice<InputLayoutAttr> attrs;
};

namespace TopologyKind {
    enum Type {
        TriangleList
    };
}
typedef TopologyKind::Type ETopologyKind;

struct Pipeline {
    ShaderRes     *shader;
    ETopologyKind topology;
    InputLayout   layout;
};

struct Bindings {
    Slice<BufferRes*> buffers;
    BufferRes *vertex   = 0;
    BufferRes *index    = 0;
    BufferRes *constant = 0;

    Slice<TextureRes*> ps_textures;
    IndexFormat::Type index_format;
};

struct Viewport {
    Vec2 pos;
    Vec2 size;
};

struct RenderPassDesc {
    TextureRes *color_attachment;
    TextureRes *depth_stencil_attachment;
};

struct RenderPass {
    void *handle;
};

enum class RAIError {
    FailedToAllocateResource = 1,
};

struct RAIGraphicsInitParams {
    void *os_context;
    Vec2i size;
    IAllocator &alloc;
};

#define PROC_RAI_CREATE_BUFFER(name) \
    Result<BufferRes, RAIError> name(BufferResDesc *desc)
#define PROC_RAI_CREATE_SHADER_PART(name) \
    Result<ShaderPartRes, RAIError> name(Raw source, EShaderPartResKind kind)
#define PROC_RAI_CREATE_SHADER(name) \
    Result<ShaderRes, RAIError> name(ShaderResDesc *desc)
#define PROC_RAI_CREATE_TEXTURE(name) \
    Result<TextureRes, RAIError> name(TextureResDesc *desc)
#define PROC_RAI_CREATE_RENDER_PASS(name) \
    RenderPass name(RenderPassDesc *desc)

#define PROC_RAI_GET_DEFAULT_TEXTURE(name) \
    TextureRes name(ETextureResUsage kind)
#define PROC_RAI_GET_DEFAULT_RENDER_PASS(name) \
    RenderPass name(void)

#define PROC_RAI_SET_PIPELINE(name) \
    bool name(const Pipeline *pipeline)
#define PROC_RAI_SET_BINDINGS(name) \
    void name(const Bindings *bindings)
#define PROC_RAI_SET_VIEWPORT(name) \
    void name(const Viewport *viewport)
#define PROC_RAI_SET_RENDER_PASS(name) \
    void name(const RenderPass *pass, Vec2i size)
#define PROC_RAI_DRAW(name) \
   void name(i32 base, i32 num_elements)
#define PROC_RAI_DRAW_INSTANCED(name) \
   void name(i32 base, u32 num_elements, u32 num_instances)
#define PROC_RAI_DRAW_INDEXED(name) \
   void name(u32 index_count, u32 start, i32 base_vertex)
#define PROC_RAI_MAP_BUFFER(name) \
    void *name(BufferRes *buffer)
#define PROC_RAI_UNMAP_BUFFER(name) \
    void name(BufferRes *buffer)
#define PROC_RAI_UPDATE_BUFFER(name) \
    void name(BufferRes *buffer, u32 offset, Raw new_data)
#define PROC_RAI_GET_BUFFER_SIZE(name) \
    u32 name(BufferRes *buffer)
#define PROC_RAI_RESIZE_BUFFER(name) \
    void name(BufferRes *buffer, u32 new_size)


typedef PROC_RAI_CREATE_BUFFER(ProcRAICreateBuffer);
typedef PROC_RAI_CREATE_SHADER_PART(ProcRAICreateShaderPart);
typedef PROC_RAI_CREATE_SHADER(ProcRAICreateShader);
typedef PROC_RAI_CREATE_TEXTURE(ProcRAICreateTexture);
typedef PROC_RAI_CREATE_RENDER_PASS(ProcRAICreateRenderPass);
typedef PROC_RAI_GET_DEFAULT_RENDER_PASS(ProcRAIGetDefaultRenderPass);

typedef PROC_RAI_UPDATE_BUFFER(ProcRAIUpdateBuffer);
typedef PROC_RAI_GET_BUFFER_SIZE(ProcRAIGetBufferSize);
typedef PROC_RAI_RESIZE_BUFFER(ProcRAIResizeBuffer);

typedef PROC_RAI_SET_PIPELINE(ProcRAISetPipeline);
typedef PROC_RAI_SET_BINDINGS(ProcRAISetBindings);
typedef PROC_RAI_SET_VIEWPORT(ProcRAISetViewport);
typedef PROC_RAI_GET_DEFAULT_TEXTURE(ProcRAIGetDefaultTexture);
typedef PROC_RAI_SET_RENDER_PASS(ProcRAISetRenderPass);
typedef PROC_RAI_DRAW(ProcRAIDraw);
typedef PROC_RAI_DRAW_INSTANCED(ProcRAIDrawInstanced);
typedef PROC_RAI_DRAW_INDEXED(ProcRAIDrawIndexed);
typedef PROC_RAI_MAP_BUFFER(ProcRAIMapBuffer);
typedef PROC_RAI_UNMAP_BUFFER(ProcRAIUnmapBuffer);

struct RAIGraphics {
    ProcRAICreateBuffer         *create_buffer;
    ProcRAICreateShaderPart     *create_shader_part;
    ProcRAICreateShader         *create_shader;
    ProcRAICreateRenderPass     *create_render_pass;
    ProcRAICreateTexture        *create_texture;
    ProcRAIGetDefaultTexture    *get_default_texture;
    ProcRAIGetDefaultRenderPass *get_default_render_pass;

    ProcRAIUpdateBuffer         *update_buffer;
    ProcRAIGetBufferSize        *get_buffer_size;
    ProcRAIResizeBuffer         *resize_buffer;

    ProcRAISetPipeline          *set_pipeline;
    ProcRAISetRenderPass        *set_render_pass;
    ProcRAISetBindings          *set_bindings;
    ProcRAISetViewport          *set_viewport;
    ProcRAIMapBuffer            *map_buffer;
    ProcRAIUnmapBuffer          *unmap_buffer;
    /** @todo: pass action (clear color, depth, stencil...) */
    ProcRAIDraw                 *draw;
    ProcRAIDrawInstanced        *draw_instanced;
    ProcRAIDrawIndexed          *draw_indexed;
};

#define PROC_RAI_INITIALIZE(name) \
    bool name(void *vparams, RAIGraphics &result)
