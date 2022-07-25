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
        RGB                 = 0x1907,
        RGBA                = 0x1908,
        BGR                 = 0x80E0,
        BGRA                = 0x80E1,
        RGBA8               = 0x8058,
        UnsignedInt         = 0x1405,
        UnsignedInt8888     = 0x8035,
        UnsignedBytes       = 0x1401,
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

static _inline void input_layout_attr_to_glprops(
    EInputLayoutAttrKind kind,
    gl::Int &size,
    gl::Enum &type,
    gl::Sizei &stride)
{
    switch (kind) {
        case InputLayoutAttrKind::Float2: {
            size = 2;
            type = gl::Float;
            stride = 2 * sizeof(gl::Float);
        } break;

        case InputLayoutAttrKind::Float3: {
            size = 3;
            type = gl::Float;
            stride = 3 * sizeof(gl::Float);
        } break;

        case InputLayoutAttrKind::Float4: {
            size = 4;
            type = gl::Float;
            stride = 4 * sizeof(gl::Float);
        } break;
    }
}

static _inline gl::Enum topology_to_gl33_topology(ETopologyKind kind) {
    static gl::Enum table[1] = {
        gl::Triangles,
    };

    return table[kind];
}

static _inline gl::Enum pixel_format_to_gl_format(
    PixelFormat::Type format,
    gl::Enum format,
    gl::Enum type)
{

    switch (format) {
        case PixelFormat::RGB8X:
            format = gl::RGB;
            type = gl::None;
            break;
        case PixelFormat::RGBA8:
            format = gl::RGBA;
            type = gl::RGBA8;
            break;
        case PixelFormat::BGRA8:
            format = gl::BGRA;
            type = gl::RGBA8;
            break;
        case PixelFormat::BGR8:
            format = gl::BGR;
            type = gl::None;
            break;
        case PixelFormat::B5G6R5:
            format = gl::RGB;
            break;
    }

    return table[format];
}