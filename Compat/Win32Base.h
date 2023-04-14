#pragma once

#define WIN32_DECLARE_HANDLE(name) \
    struct name##__ {              \
        int unused;                \
    };                             \
    typedef struct name##__* name
#define WIN32_DECLARE_WNDPROC(name) \
    Win32::LRESULT name(            \
        Win32::HWND   hwnd,         \
        Win32::UINT   msg,          \
        Win32::WPARAM wparam,       \
        Win32::LPARAM lparam)
#define WIN32_MAKEINTRESOURCEA(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#define WIN32_MAKEINTRESOURCEW(i) ((LPWSTR)((ULONG_PTR)((WORD)(i))))
#define WIN32_LOWORD(l) ((Win32::WORD)(((Win32::DWORD_PTR)(l)) & 0xffff))
#define WIN32_HIWORD(l) \
    ((Win32::WORD)((((Win32::DWORD_PTR)(l)) >> 16) & 0xffff))
#define WIN32_GET_X_LPARAM(lp) ((int)(short)WIN32_LOWORD(lp))
#define WIN32_GET_Y_LPARAM(lp) ((int)(short)WIN32_HIWORD(lp))

namespace Win32 {
    WIN32_DECLARE_HANDLE(HINSTANCE);
    WIN32_DECLARE_HANDLE(HRAWINPUT);
    typedef HINSTANCE HMODULE;

    typedef unsigned long    DWORD;
    typedef int              BOOL;
    typedef int              INT;
    typedef long             LONG;
    typedef unsigned int     UINT;
    typedef UINT*            PUINT;
    typedef unsigned short   USHORT;
    typedef unsigned char    BYTE;
    typedef unsigned short   WORD;
    typedef float            FLOAT;
    typedef long long        INT_PTR;
    typedef char             CHAR;
    typedef CHAR*            LPSTR;
    typedef const CHAR*      LPCSTR;
    typedef wchar_t          WCHAR;
    typedef WCHAR*           LPWSTR;
    typedef const WCHAR*     LPCWSTR;
    typedef unsigned long    ULONG;
    typedef unsigned __int64 UINT_PTR, *PUINT_PTR;
    typedef __int64          LONG_PTR, *PLONG_PTR;
    typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
    typedef UINT_PTR         WPARAM;
    typedef LONG_PTR         LPARAM;
    typedef LONG_PTR         LRESULT;
    typedef void*            LPVOID;
    typedef void*            PVOID;
    typedef PVOID            HANDLE;
    typedef ULONG_PTR        DWORD_PTR, *PDWORD_PTR;
    typedef LONG             HRESULT;
    typedef WORD             ATOM;

    WIN32_DECLARE_HANDLE(HICON);
    WIN32_DECLARE_HANDLE(HBRUSH);
    WIN32_DECLARE_HANDLE(HWND);
    WIN32_DECLARE_HANDLE(HMENU);
    typedef HICON HCURSOR;

    typedef LRESULT(__stdcall* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

    struct WNDCLASSEXA {
        UINT      size;
        UINT      style;
        WNDPROC   wndproc;
        int       cls_extra;
        int       wnd_extra;
        HINSTANCE instance;
        HICON     icon;
        HCURSOR   cursor;
        HBRUSH    background;
        LPCSTR    menu_name;
        LPCSTR    class_name;
        HICON     small_icon;
    };

    typedef struct tagPOINT {
        LONG x;
        LONG y;
    } POINT, *PPOINT, *LPPOINT;

    typedef struct tagMSG {
        HWND   hwnd;
        UINT   message;
        WPARAM wParam;
        LPARAM lParam;
        DWORD  time;
        POINT  pt;
#ifdef _MAC
        DWORD lPrivate;
#endif
    } MSG, *PMSG, *LPMSG;

    typedef struct tagRECT {
        LONG left;
        LONG top;
        LONG right;
        LONG bottom;
    } RECT, *PRECT, *NPRECT, *LPRECT;

    typedef struct tagRAWINPUTDEVICE {
        USHORT usUsagePage;
        USHORT usUsage;
        DWORD  dwFlags;
        HWND   hwndTarget;
    } RAWINPUTDEVICE, *PRAWINPUTDEVICE, *LPRAWINPUTDEVICE;

    typedef struct tagRAWINPUTHEADER {
        DWORD  dwType;
        DWORD  dwSize;
        HANDLE hDevice;
        WPARAM wParam;
    } RAWINPUTHEADER, *PRAWINPUTHEADER, *LPRAWINPUTHEADER;

    typedef struct tagRAWMOUSE {
        USHORT usFlags;
        union {
            ULONG ulButtons;
            struct {
                USHORT usButtonFlags;
                USHORT usButtonData;
            } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;
        ULONG ulRawButtons;
        LONG  lLastX;
        LONG  lLastY;
        ULONG ulExtraInformation;
    } RAWMOUSE, *PRAWMOUSE, *LPRAWMOUSE;

    typedef struct tagRAWKEYBOARD {
        USHORT MakeCode;
        USHORT Flags;
        USHORT Reserved;
        USHORT VKey;
        UINT   Message;
        ULONG  ExtraInformation;
    } RAWKEYBOARD, *PRAWKEYBOARD, *LPRAWKEYBOARD;

    typedef struct tagRAWHID {
        DWORD dwSizeHid;
        DWORD dwCount;
        BYTE  bRawData[1];
    } RAWHID, *PRAWHID, *LPRAWHID;

    typedef struct tagRAWINPUT {
        RAWINPUTHEADER header;
        union {
            RAWMOUSE    mouse;
            RAWKEYBOARD keyboard;
            RAWHID      hid;
        } data;
    } RAWINPUT, *PRAWINPUT, *LPRAWINPUT;

#ifndef MOK_WIN32_NO_FUNCTIONS
    extern "C" __declspec(dllimport) DWORD __stdcall GetLastError(void);
    extern "C" __declspec(dllimport) void __stdcall SetLastError(DWORD code);
    extern "C" __declspec(dllimport) ATOM
        __stdcall RegisterClassExA(WNDCLASSEXA* wnd_class);
    extern "C" __declspec(dllimport) HWND __stdcall CreateWindowExA(
        DWORD     ex_style,
        LPCSTR    class_name,
        LPCSTR    window_name,
        DWORD     style,
        int       x,
        int       y,
        int       w,
        int       h,
        HWND      parent,
        HMENU     menu,
        HINSTANCE instance,
        LPVOID    param);
    extern "C" __declspec(dllimport) BOOL DestroyWindow(HWND window);
    extern "C" __declspec(dllimport) BOOL
        __stdcall ShowWindow(HWND handle, int cmd_show);
    extern "C" __declspec(dllimport) BOOL __stdcall DestroyWindow(HWND handle);
    extern "C" __declspec(dllimport) void __stdcall PostQuitMessage(
        int exit_code);
    extern "C" __declspec(dllimport) LRESULT __stdcall DefWindowProcA(
        HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);
    extern "C" __declspec(dllimport) HCURSOR
        __stdcall LoadCursorA(HINSTANCE instance, LPCSTR name);
    extern "C" __declspec(dllimport) HCURSOR
        __stdcall LoadCursorW(HINSTANCE instance, LPCWSTR name);
    extern "C" __declspec(dllimport) HMODULE
        __stdcall GetModuleHandleA(LPCSTR module_name);
    extern "C" __declspec(dllimport) HMODULE
        __stdcall GetModuleHandleW(LPCWSTR module_name);
    extern "C" __declspec(dllimport) BOOL __stdcall UpdateWindow(HWND hwnd);
    extern "C" __declspec(dllimport) LONG_PTR
        __stdcall SetWindowLongPtrA(HWND hwnd, int index, LONG_PTR ptr);
    extern "C" __declspec(dllimport) LONG_PTR
        __stdcall SetWindowLongPtrW(HWND hwnd, int index, LONG_PTR ptr);
    extern "C" __declspec(dllimport) LONG_PTR
        __stdcall GetWindowLongPtrA(HWND hwnd, int index);
    extern "C" __declspec(dllimport) LONG_PTR
        __stdcall GetWindowLongPtrW(HWND hwnd, int index);
    extern "C" __declspec(dllimport) BOOL __stdcall PeekMessageA(
        LPMSG msg,
        HWND  hwnd,
        UINT  filter_min,
        UINT  filter_max,
        UINT  remove_msg);
    extern "C" __declspec(dllimport) BOOL __stdcall PeekMessageW(
        LPMSG msg,
        HWND  hwnd,
        UINT  filter_min,
        UINT  filter_max,
        UINT  remove_msg);
    extern "C" __declspec(dllimport) BOOL
        __stdcall TranslateMessage(const MSG* msg);
    extern "C" __declspec(dllimport) LRESULT
        __stdcall DispatchMessageA(const MSG* msg);
    extern "C" __declspec(dllimport) LRESULT
        __stdcall DispatchMessageW(const MSG* msg);
    extern "C" BOOL GetWindowRect(HWND hWnd, LPRECT lpRect);
    extern "C" BOOL GetClientRect(HWND hWnd, LPRECT lpRect);
    extern "C" BOOL ClientToScreen(HWND hWnd, LPPOINT lpPoint);
    extern "C" BOOL ScreenToClient(HWND hWnd, LPPOINT lpPoint);
    extern "C" BOOL AdjustWindowRect(LPRECT rect, DWORD style, BOOL menu);
    extern "C" BOOL SetCursorPos(int X, int Y);
    extern "C" int  ShowCursor(BOOL bShow);
    extern "C" HWND SetCapture(HWND hWnd);
    extern "C" BOOL ReleaseCapture();

    extern "C" BOOL RegisterRawInputDevices(
        PRAWINPUTDEVICE pRawInputDevices, UINT uiNumDevices, UINT cbSize);
    extern "C" UINT GetRawInputData(
        HRAWINPUT hRawInput,
        UINT      uiCommand,
        LPVOID    pData,
        PUINT     pcbSize,
        UINT      cbSizeHeader);

    extern "C" BOOL SetProcessDPIAware();

#endif  // MOK_WIN32_NO_FUNCTIONS

    constexpr int DefaultWindowPos = ((int)0x80000000);
    namespace HIDUsage {
        constexpr unsigned short Mouse = 0x02;
    }  // namespace HIDUsage

    namespace HIDUsagePage {
        constexpr unsigned short Generic = 0x01;
    }  // namespace HIDUsagePage

    namespace HIDMode {
        /* If set, this enables the caller to receive the input even when the
         * caller is not in the foreground. Note that hwndTarget must be
         * specified. */
        constexpr UINT InputSink = 0x00000100;
    }  // namespace HIDMode

    namespace RawInputCommand {
        constexpr UINT Header = 0x10000005;
        constexpr UINT Input  = 0x10000003;
    }  // namespace RawInputCommand

    namespace RawInputDataType {
        constexpr DWORD Mouse    = 0;
        constexpr DWORD Keyboard = 1;
        constexpr DWORD Hid      = 2;
    }  // namespace RawInputDataType

    namespace RawInputMouseState {
        constexpr USHORT MoveRelative      = 0x00;
        constexpr USHORT MoveAbsolute      = 0x01;
        constexpr USHORT VirtualDesktop    = 0x02;
        constexpr USHORT AttributesChanged = 0x04;
        constexpr USHORT NoCoalesce        = 0x08;

    }  // namespace RawInputMouseState

    namespace PeekMessageOption {
        constexpr UINT NoRemove = 0x0000;
        constexpr UINT Remove   = 0x0001;
        constexpr UINT NoYield  = 0x0002;
    }  // namespace PeekMessageOption

    namespace WindowLongPointer {
        constexpr int WndProc    = -4;
        constexpr int HInstance  = -6;
        constexpr int HWNDParent = -8;
        constexpr int UserData   = -21;
        constexpr int ID         = -12;
    }  // namespace WindowLongPointer

    namespace ShowMode {
        constexpr int Hide            = 0;
        constexpr int ShowNormal      = 1;
        constexpr int Normal          = 1;
        constexpr int ShowMinimized   = 2;
        constexpr int ShowMaximized   = 3;
        constexpr int Maximize        = 3;
        constexpr int ShowNoActivate  = 4;
        constexpr int Show            = 5;
        constexpr int Minimize        = 6;
        constexpr int ShowMinNoActive = 7;
        constexpr int ShowNa          = 8;
        constexpr int Restore         = 9;
        constexpr int ShowDefault     = 10;
        constexpr int ForceMinimize   = 11;
        constexpr int Max             = 11;
    }  // namespace ShowMode

    namespace Style {
        constexpr long Overlapped   = 0x00000000L;
        constexpr long Popup        = 0x80000000L;
        constexpr long Child        = 0x40000000L;
        constexpr long Minimize     = 0x20000000L;
        constexpr long Visible      = 0x10000000L;
        constexpr long Disabled     = 0x08000000L;
        constexpr long ClipSiblings = 0x04000000L;
        constexpr long ClipChildren = 0x02000000L;
        constexpr long Maximize     = 0x01000000L;
        constexpr long Caption      = 0x00C00000L;
        constexpr long Border       = 0x00800000L;
        constexpr long DlgFrame     = 0x00400000L;
        constexpr long VScroll      = 0x00200000L;
        constexpr long HScroll      = 0x00100000L;
        constexpr long SysMenu      = 0x00080000L;
        constexpr long ThickFrame   = 0x00040000L;
        constexpr long Group        = 0x00020000L;
        constexpr long TabStop      = 0x00010000L;
        constexpr long MinimizeBox  = 0x00020000L;
        constexpr long MaximizeBox  = 0x00010000L;

        constexpr long OverlappedWindow =
            Overlapped | Caption | SysMenu | ThickFrame | MinimizeBox |
            MaximizeBox;
        /**
         * An overlapped window without the ability to be resized or maximized
         */
        constexpr long OverlappedWindowNonResizeable =
            OverlappedWindow & (~MaximizeBox) & (~ThickFrame);
    }  // namespace Style

    namespace Cursor {
        const LPSTR  A_Arrow    = WIN32_MAKEINTRESOURCEA(32512);
        const LPSTR  A_IBeam    = WIN32_MAKEINTRESOURCEA(32513);
        const LPSTR  A_Wait     = WIN32_MAKEINTRESOURCEA(32514);
        const LPSTR  A_Cross    = WIN32_MAKEINTRESOURCEA(32515);
        const LPSTR  A_UpArrow  = WIN32_MAKEINTRESOURCEA(32516);
        const LPSTR  A_Size     = WIN32_MAKEINTRESOURCEA(32640);
        const LPSTR  A_Icon     = WIN32_MAKEINTRESOURCEA(32641);
        const LPSTR  A_SizeNWSE = WIN32_MAKEINTRESOURCEA(32642);
        const LPSTR  A_SizeNESW = WIN32_MAKEINTRESOURCEA(32643);
        const LPSTR  A_SizeWE   = WIN32_MAKEINTRESOURCEA(32644);
        const LPSTR  A_SizeNS   = WIN32_MAKEINTRESOURCEA(32645);
        const LPSTR  A_SizeAll  = WIN32_MAKEINTRESOURCEA(32646);
        const LPSTR  A_No       = WIN32_MAKEINTRESOURCEA(32648);
        const LPWSTR W_Arrow    = WIN32_MAKEINTRESOURCEW(32512);
        const LPWSTR W_IBeam    = WIN32_MAKEINTRESOURCEW(32513);
        const LPWSTR W_Wait     = WIN32_MAKEINTRESOURCEW(32514);
        const LPWSTR W_Cross    = WIN32_MAKEINTRESOURCEW(32515);
        const LPWSTR W_UpArrow  = WIN32_MAKEINTRESOURCEW(32516);
        const LPWSTR W_Size     = WIN32_MAKEINTRESOURCEW(32640);
        const LPWSTR W_Icon     = WIN32_MAKEINTRESOURCEW(32641);
        const LPWSTR W_SizeNWSE = WIN32_MAKEINTRESOURCEW(32642);
        const LPWSTR W_SizeNESW = WIN32_MAKEINTRESOURCEW(32643);
        const LPWSTR W_SizeWE   = WIN32_MAKEINTRESOURCEW(32644);
        const LPWSTR W_SizeNS   = WIN32_MAKEINTRESOURCEW(32645);
        const LPWSTR W_SizeAll  = WIN32_MAKEINTRESOURCEW(32646);
        const LPWSTR W_No       = WIN32_MAKEINTRESOURCEW(32648);
    }  // namespace Cursor

    namespace Message {
        constexpr UINT Null      = 0x0000;
        constexpr UINT Create    = 0x0001;
        constexpr UINT Destroy   = 0x0002;
        constexpr UINT Move      = 0x0003;
        constexpr UINT Size      = 0x0005;
        constexpr UINT Activate  = 0x0006;
        constexpr UINT KeyDown   = 0x0100;
        constexpr UINT KeyUp     = 0x0101;
        constexpr UINT MouseMove = 0x0200;
        constexpr UINT Input     = 0x00FF;
    }  // namespace Message

    namespace HResult {
        constexpr HRESULT Ok    = ((HRESULT)0L);
        constexpr HRESULT False = ((HRESULT)1L);
    }  // namespace HResult

}  // namespace Win32
