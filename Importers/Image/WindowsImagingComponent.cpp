#include "WindowsImagingComponent.h"
#include "FileSystem/Extras.h"
#include "Importers/Image/Image.h"
#include "Memory/Arena.h"
#include <objbase.h>
#include <wincodec.h>
#include <shlwapi.h>
#include "Importers/Import.h"

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "shlwapi.lib")

// @bug: image is interpreted wrong, probably endianess pixel format issue
PROC_IMPORTER_LOAD(import_wic_load) {

    HRESULT hr = CoInitialize(0);

    IWICImagingFactory *imaging_factory;
    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWICImagingFactory,
        (void**)&imaging_factory);
    if (hr != S_OK) {
        return false;
    }

    DEFER(imaging_factory->Release());


    WICDecodeOptions decode_options = WICDecodeMetadataCacheOnDemand;

    IWICBitmapDecoder *decoder;
    hr = imaging_factory->CreateDecoderFromFileHandle(
        (u64)file_handle.internal_handle,
        0,
        decode_options,
        &decoder);

    if (hr != S_OK) {
        return false;
    }

    DEFER(decoder->Release());

    IWICBitmapFrameDecode *frame;
    hr = decoder->GetFrame(0, &frame);
    if (hr != S_OK) {
        return false;
    }
    DEFER(frame->Release());

    // @todo: check native pixel formats and convert only if needed

    IWICFormatConverter *format_converter;
    hr = imaging_factory->CreateFormatConverter(&format_converter);
    if (hr != S_OK) {
        return false;
    }

    DEFER(format_converter->Release());

    hr = format_converter->Initialize(
        frame,
        GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone,
        0,
        0,
        WICBitmapPaletteTypeCustom);

    if (hr != S_OK) {
        return false;
    }

    u32 width, height;
    format_converter->GetSize(&width, &height);

    result->kind = ImportKind::Image;

    result->data.size = width * height * 4;
    result->data.buffer = alloc->reserve(result->data.size);

    result->image.bpp        = 32;
    result->image.format     = PixelFormat::RGBA8;
    result->image.is_flipped = false;
    result->image.pitch      = width * 4;
    result->image.width      = width;
    result->image.height     = height;

    hr = format_converter->CopyPixels(
        0,
        width * 4,
        (UINT)result->data.size,
        (u8*)result->data.buffer);

    if (hr != S_OK) {
        alloc->release((umm)result->data.buffer);
        result->data.buffer = 0;
        return false;
    }

    return true;
}
