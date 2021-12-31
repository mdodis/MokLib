#include "WindowsImagingComponent.h"
#include "FileSystem/Extras.h"
#include "Memory/Arena.h"
#include <objbase.h>
#include <wincodec.h>
#include <shlwapi.h>

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "shlwapi.lib")

PROC_IMPORTER_LOAD(import_wic_load) {

    HRESULT hr = CoInitialize(0);

    if (hr != S_OK) {
        return false;
    }

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


}
