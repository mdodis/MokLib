#pragma once
#include "Base.h"
#include <cstdio>
// #include "WinInc.h"

namespace ConsoleColor {
    enum Type {
        Red     = 0x0000,
        Green   = 0x0004,
        Blue    = 0x0008,
        White   = 0x000C,

        Invalid = 0x0010,
        Count
    };

    enum {
        Bold = 0x1,
        Underline = 0x2,
    };

};

/**
 * Windows
 */

#if OS_WINDOWS
namespace Console {
    enum Handle {
        Input  = -10,
        Output = -11,
        Error  = -12,
    };
};

// Win32 Console color definitions
#define WIN32_FG_BLUE                       0x0001 // text color contains blue.
#define WIN32_FG_GREEN                      0x0002 // text color contains green.
#define WIN32_FG_RED                        0x0004 // text color contains red.
#define WIN32_FG_INT                        0x0008 // text color is intensified.
#define WIN32_BACKGROUND_BLUE               0x0010 // background color contains blue.
#define WIN32_BACKGROUND_GREEN              0x0020 // background color contains green.
#define WIN32_BACKGROUND_RED                0x0040 // background color contains red.
#define WIN32_BACKGROUND_INTENSITY          0x0080 // background color is intensified.
#define WIN32_COMMON_LVB_LEADING_BYTE       0x0100 // Leading Byte of DBCS
#define WIN32_COMMON_LVB_TRAILING_BYTE      0x0200 // Trailing Byte of DBCS
#define WIN32_COMMON_LVB_GRID_HORIZONTAL    0x0400 // DBCS: Grid attribute: top horizontal.
#define WIN32_COMMON_LVB_GRID_LVERTICAL     0x0800 // DBCS: Grid attribute: left vertical.
#define WIN32_COMMON_LVB_GRID_RVERTICAL     0x1000 // DBCS: Grid attribute: right vertical.
#define WIN32_COMMON_LVB_REVERSE_VIDEO      0x4000 // DBCS: Reverse fore/back ground attribute.
#define WIN32_COMMON_LVB_UNDERSCORE         0x8000 // DBCS: Underscore.
#define WIN32_COMMON_LVB_SBCSDBCS           0x0300 // SBCS or DBCS flag.

extern "C" __declspec(dllimport) int __stdcall SetConsoleTextAttribute(void *hConsoleOutput, unsigned short wAttributes);

extern "C" __declspec(dllimport) void * __stdcall GetStdHandle(unsigned long nStdHandle);

static uint16 Color_Translation_Table[ConsoleColor::Count] = {
    /* Red                  */ WIN32_FG_RED,
    /* Red Bold             */ WIN32_FG_RED | WIN32_FG_INT,
    /* Red Underline        */ WIN32_FG_RED | WIN32_COMMON_LVB_UNDERSCORE,
    /* Red Bold Underline   */ WIN32_FG_RED | WIN32_FG_INT | WIN32_COMMON_LVB_UNDERSCORE,
    /* Green                */ WIN32_FG_GREEN,
    /* Green Bold           */ WIN32_FG_GREEN | WIN32_FG_INT,
    /* Green Underline      */ WIN32_FG_GREEN | WIN32_COMMON_LVB_UNDERSCORE,
    /* Green Bold Underline */ WIN32_FG_GREEN | WIN32_FG_INT | WIN32_COMMON_LVB_UNDERSCORE,
    /* Blue                 */ WIN32_FG_BLUE,
    /* Blue Bold            */ WIN32_FG_BLUE | WIN32_FG_INT,
    /* Blue Underline       */ WIN32_FG_BLUE | WIN32_COMMON_LVB_UNDERSCORE,
    /* Blue Bold Underline  */ WIN32_FG_BLUE | WIN32_FG_INT | WIN32_COMMON_LVB_UNDERSCORE,
    /* White                */ WIN32_FG_RED | WIN32_FG_GREEN | WIN32_FG_BLUE,
    /* White Bold           */ WIN32_FG_RED | WIN32_FG_GREEN | WIN32_FG_BLUE | WIN32_FG_INT,
    /* White Underline      */ WIN32_FG_RED | WIN32_FG_GREEN | WIN32_FG_BLUE | WIN32_COMMON_LVB_UNDERSCORE,
    /* White Bold Underline */ WIN32_FG_RED | WIN32_FG_GREEN | WIN32_FG_BLUE | WIN32_FG_INT | WIN32_COMMON_LVB_UNDERSCORE,
    /* Default */              WIN32_FG_RED | WIN32_FG_GREEN | WIN32_FG_BLUE
};

namespace Console {
    static _inline void set_color(Console::Handle handle, ConsoleColor::Type color = ConsoleColor::Invalid) {
        SetConsoleTextAttribute(GetStdHandle((unsigned long)handle), Color_Translation_Table[color]);
    }
};

#endif


/**
 * Linux
 */

#if OS_LINUX

namespace Console {
    enum Handle {
        Input = 1,
        Output,
        Error
    };
}

static const char *Color_Translation_Table[ConsoleColor::Count] = {
    /* Red                  */ "\e[31m",
    /* Red Bold             */ "\e[1;31m",
    /* Red Underline        */ "\e[4;31m",
    /* Red Bold Underline   */ "\e[1;4;31m",
    /* Green                */ "\e[32m",
    /* Green Bold           */ "\e[1;32m",
    /* Green Underline      */ "\e[4;32m",
    /* Green Bold Underline */ "\e[1;4;32m",
    /* Blue                 */ "\e[34m",
    /* Blue Bold            */ "\e[1;34m",
    /* Blue Underline       */ "\e[4;34m",
    /* Blue Bold Underline  */ "\e[1;4;34m",
    /* White                */ "\e[31m",
    /* White Bold           */ "\e[1;31m",
    /* White Underline      */ "\e[4;31m",
    /* White Bold Underline */ "\e[1;4;31m",
    /* Default */              "\e[0m"
};

namespace Console {

    static _inline void set_color(Console::Handle handle, ConsoleColor::Type color = ConsoleColor::Invalid) {

        FILE *file;

        switch (handle) {
            case Input: {
                file = stdin;
            } break;

            case Output: {
                file = stdout;
            } break;

            case Error: {
                file = stderr;
            } break;
        }

        fputs(Color_Translation_Table[color], file);
    }
};

#endif