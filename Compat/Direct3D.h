#pragma once
#include "Base.h"
#include "Host.h"
#include "Importers/Import.h"
#include "Memory/RawBuffer.h"
#include <d3d11.h>
#include <dxgiformat.h>

#if !OS_WINDOWS
#error "Direct3D.h is not meant for anything other windows!"
#endif

static _inline DXGI_FORMAT pixel_format_to_dxgi_format(PixelFormat::Type format) {

    static DXGI_FORMAT table[PixelFormat::Count] = {
        DXGI_FORMAT_UNKNOWN,        // RGB8
        DXGI_FORMAT_R8G8B8A8_UINT, // RGBA8
        DXGI_FORMAT_B8G8R8A8_UNORM, // BGRA8
        DXGI_FORMAT_UNKNOWN,        // BGR8
        DXGI_FORMAT_B5G6R5_UNORM,   // B5G6R5
    };

    return table[format];
}

static _inline D3D11_SUBRESOURCE_DATA raw_to_subresource_data(const RawTexture &data) {
    D3D11_SUBRESOURCE_DATA result = {};
    result.pSysMem = data.buffer;
    result.SysMemPitch = data.pitch;
    return result;
}