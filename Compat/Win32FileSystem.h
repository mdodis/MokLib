#pragma once
#include "Base.h"
#include "FileSystem/FileSystem.h"

namespace Win32FileAttributes {
    enum Type : u32 {
        ReadOnly           = 0x1,
        Hidden             = 0x2,
        System             = 0x4,
        Directory          = 0x10,
        Archive            = 0x20,
        Device             = 0x40,
        Normal             = 0x80,
        Temporary          = 0x100,
        SparseFile         = 0x200,
        ReparsePoint       = 0x400,
        Compressed         = 0x800,
        Offline            = 0x1000,
        Encrypted          = 0x4000,
        IntegrityStream    = 0x8000,
        NotContentIndexed  = 0x2000,
        NoScrubData        = 0x20000,
        Virtual            = 0x10000,
        RecallOnOpen       = 0x40000,
        RecallOnDataAccess = 0x400000,
    };
};

static _inline EFileAttributes win32_file_attribs_to_file_attribs(Win32FileAttributes::Type attrib) {
    EFileAttributes result;

    switch (attrib) {

        case Win32FileAttributes::Normal:    result = FileAttributes::File; break;
        case Win32FileAttributes::Directory: result = FileAttributes::Directory; break;

        default: {
            result = 0;
        } break;
    }

    return result;
}