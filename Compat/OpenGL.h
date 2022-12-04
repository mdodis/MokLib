#pragma once
#include "Base.h"
#include "Platform/RAI.h"

namespace gl {
    typedef unsigned int UInt;
    typedef unsigned int Enum;
    typedef int Sizei;
    typedef int Int;
    typedef char Bool;
    typedef char Char;

    // Win64: signed long long int
    typedef signed long int IntPtr;
    typedef signed long int SizeiPtr;

    enum Constants : u32 {
        None                = 0,
        ArrayBuffer         = 0x8892,
        ElementArrayBuffer  = 0x8893,
        UniformBuffer       = 0x8A11,
        DepthBufferBit      = 0x00000100,
        StencilBufferBit    = 0x00000400,
        ColorBufferBit      = 0x00004000,
        Framebuffer         = 0x8D40,
        StreamDraw          = 0x88E0,
        StreamRead          = 0x88E1,
        StreamCopy          = 0x88E2,
        StaticDraw          = 0x88E4,
        StaticRead          = 0x88E5,
        StaticCopy          = 0x88E6,
        DynamicDraw         = 0x88E8,
        DynamicRead         = 0x88E9,
        DynamicCopy         = 0x88EA,
        VertexShader        = 0x8B31,
        FragmentShader      = 0x8B30,
        CompileStatus       = 0x8B81,
        LinkStatus          = 0x8B82,
        Float               = 0x1406,
        Triangles           = 0x0004,
        ReadWrite           = 0x88BA,
        WriteOnly           = 0x88B9,
        Texture2D           = 0x0DE1,
        Rgb                 = 0x1907,
        Rgba                = 0x1908,
        Bgr                 = 0x80E0,
        Bgra                = 0x80E1,
        Rgba8               = 0x8058,
        UnsignedInt         = 0x1405,
        UnsignedInt8888     = 0x8035,
        UnsignedBytes       = 0x1401,
        UnsignedShort565    = 0x8363,
        Texture0            = 0x84C0,
        UnpackAlignment     = 0x0CF5,
        TextureMagFilter    = 0x2800,
        TextureMinFilter    = 0x2801,
        Linear              = 0x2601,
        CullFace            = 0x0B44,
        BufferSize          = 0x8764,
        Integer             = 0x1404,
        Blend               = 0x0BE2,
        SrcAlpha            = 0x0302,
        OneMinusSrcAlpha    = 0x0303,
        DepthTest           = 0x0B71,
        Alpha               = 0x1906,
        Red                 = 0x1903,
    };

    enum Errors : u32 {
        InvalidEnum         = 0x0500,
        InvalidValue        = 0x0501,
        InvalidOperation    = 0x0502,
    };
}

static _inline gl::Enum buffer_res_kind_to_glenum(EBufferResKind kind) {
    static gl::Enum table[3] = {
        gl::ArrayBuffer,        // Vertex
        gl::ElementArrayBuffer, // Index
        gl::UniformBuffer,      // Constant
    };
    return table[kind];
}

static _inline gl::Enum shader_part_kind_to_glenum(EShaderPartResKind kind) {
    static gl::Enum table[2] = {
        gl::VertexShader,   // Vertex Shader
        gl::FragmentShader, // Pixel Shader
    };
    return table[kind];
}


namespace VertexAttribPointerVariantHint {
    enum Type {
        // Use glVertexAttribPointer: When normalization is enabled, data will 
        // be mapped to either [-1, 1] or [0, 1], depending on signage. 
        // Otherwise, all values will be converted to floating-point values
        Default,
        // Use glVertexAttribIPointer: Only accepts integer values, and data
        // will be passed as integers (i.e. no conversion to floats).
        Integer,
        // Use glVertexAttribLPointer: Only accepts doubles.
        Long
    };
}
typedef VertexAttribPointerVariantHint::Type EVertexAttribPointerVariantHint;

static _inline EVertexAttribPointerVariantHint input_layout_attr_to_glprops(
    EInputLayoutAttrKind kind,
    gl::Int &size,
    gl::Enum &type,
    gl::Sizei &istride)
{
    switch (kind) {
        case InputLayoutAttrKind::None: {
            size = 0;
            type = gl::None;
            istride = 0;
            return VertexAttribPointerVariantHint::Default;
        } break;

        case InputLayoutAttrKind::Int: {
            size = 1;
            type = gl::Integer;
            istride = sizeof(gl::Int);
            return VertexAttribPointerVariantHint::Integer;
        } break;

        case InputLayoutAttrKind::Float: {
            size = 1;
            type = gl::Float;
            istride = sizeof(gl::Float);
            return VertexAttribPointerVariantHint::Default;
        } break;

        case InputLayoutAttrKind::Float2: {
            size = 2;
            type = gl::Float;
            istride = 2 * sizeof(gl::Float);
            return VertexAttribPointerVariantHint::Default;
        } break;

        case InputLayoutAttrKind::Float3: {
            size = 3;
            type = gl::Float;
            istride = 3 * sizeof(gl::Float);
            return VertexAttribPointerVariantHint::Default;
        } break;

        case InputLayoutAttrKind::Float4: {
            size = 4;
            type = gl::Float;
            istride = 4 * sizeof(gl::Float);
            return VertexAttribPointerVariantHint::Default;
        } break;

        case InputLayoutAttrKind::Int4: {
            size = 4;
            type = gl::Integer;
            istride = 0;
            return VertexAttribPointerVariantHint::Integer;
        } break;
    }
}

static _inline gl::Enum topology_to_gl33_topology(ETopologyKind kind) {
    static gl::Enum table[1] = {
        gl::Triangles,
    };

    return table[kind];
}

static _inline void pixel_format_to_gl_format(
    PixelFormat::Type pformat,
    gl::Enum &format,
    gl::Enum &type,
    gl::Enum &internal_format)
{

    static gl::Enum format_to_gl[PixelFormat::Count * 3] = {
        gl::Rgb,       gl::UnsignedInt8888,  gl::Rgb,  // RGB8X
        gl::Rgba,      gl::UnsignedInt8888,  gl::Rgba, // RGBA8
        gl::Bgra,      gl::UnsignedInt8888,  gl::Rgba, // BGRA8
        gl::Bgr,       gl::None,             gl::Rgb,  // BGR8
        gl::Bgr,       gl::UnsignedShort565, gl::Rgb,  // B5G6R5
        gl::Rgb,       gl::UnsignedBytes,    gl::Rgb,  // Mono8
        gl::Red,       gl::UnsignedBytes,    gl::Red,  // Alpha8
    };

    format = format_to_gl[pformat * 3];
    type   = format_to_gl[pformat * 3 + 1];
    internal_format = format_to_gl[pformat * 3 + 2];
}