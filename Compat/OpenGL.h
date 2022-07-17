#pragma once

namespace gl {
    typedef unsigned int UInt;
    typedef unsigned int Enum;
    typedef int Sizei;
    typedef int Int;
    typedef char Bool;

    enum Constants : u32 {
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
    };
}


// #define GL_POINTS                         0x0000
// #define GL_LINES                          0x0001
// #define GL_LINE_LOOP                      0x0002
// #define GL_LINE_STRIP                     0x0003
// #define GL_TRIANGLES                      0x0004
// #define GL_TRIANGLE_STRIP                 0x0005
// #define GL_TRIANGLE_FAN                   0x0006
// #define GL_QUADS                          0x0007
// #define GL_NEVER                          0x0200
// #define GL_LESS                           0x0201
// #define GL_EQUAL                          0x0202
// #define GL_LEQUAL                         0x0203
// #define GL_GREATER                        0x0204
// #define GL_NOTEQUAL                       0x0205
// #define GL_GEQUAL                         0x0206