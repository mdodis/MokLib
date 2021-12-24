#ifndef MOK_COMPAT_OPENGL_H
#define MOK_COMPAT_OPENGL_H
#include "Base.h"
#include "Debugging/Base.h"
#include "Defer.h"
#include "Str.h"

#if OS_WINDOWS
#include "WinInc.h"
#include <gl/GL.h>
typedef char GLchar;
typedef GLsizei* GLsizeiptr;
#elif OS_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <GL/gl.h>
#include <GL/glx.h>
#endif

#define GL_ALL_PROCS \
GLPROC(void, UseProgram, GLuint program);

#define GLPROC(rett, name, ...) typedef rett(*ProcGL##name)(__VA_ARGS__); extern ProcGL##name gl##name;

GL_ALL_PROCS
#undef GLPROC

namespace GLProfileFlag {
    enum : i32 {
        CoreProfile = 1 << 0,
    };
};
typedef i32 EGLProfileFlags;

struct GLContextCreationFormatDesc {
    u8 color_bits;                  // In
    u8 alpha_bits;                  // In
    u8 depth_bits;                  // In
    u8 stencil_bits;                // In
    i32 major_version;              // In
    i32 minor_version;              // In
    EGLProfileFlags profile_flags;  // In
    void *handle;                   // In
    bool swap_interval;             // Out: true if swap_interval is supported
};

bool create_gl_context(GLContextCreationFormatDesc *desc);
bool gl_swap_interval(i32 interval);
void *gl_get_proc_address(const char *name);
bool gl_load_procs();
void gl_swap_buffers(void *window_handle);

#endif // MOK_COMPAT_OPENGL_H

#if !defined(MOK_COMPAT_OPENGL_IMPLEMENTATION)
#if OS_WINDOWS

#define PROC_WGL_CREATE_CONTEXT_ATTRIBS_ARB(name)   HGLRC name(HDC dc, HGLRC share_context, const i32 *attrib_list)
#define PROC_WGL_GET_EXTENSIONS_STRING_ARB(name)    const char *name(HDC dc)
#define PROC_WGL_SWAP_INTERVAL_EXT(name)            i32 name(i32 interval)
#define PROC_WGL_CHOOSE_PIXEL_FORMAT_ARGB(name)     i32 name(HDC dc, const i32 *attribs_i, const float *attribs_f, u32 max_formats, i32 *formats, u32 *num_formats)

typedef PROC_WGL_CREATE_CONTEXT_ATTRIBS_ARB(ProcWGLCreateContextAttribsARB);
typedef PROC_WGL_GET_EXTENSIONS_STRING_ARB(ProcWGLGetExtensionsStringARB);
typedef PROC_WGL_SWAP_INTERVAL_EXT(ProcWGLSwapIntervalEXT);
typedef PROC_WGL_CHOOSE_PIXEL_FORMAT_ARGB(ProcWGLChoosePixelFormatARB);

static struct {
    ProcWGLCreateContextAttribsARB *create_context_attribs_arb;
    ProcWGLGetExtensionsStringARB  *get_extensions_string_arb;
    ProcWGLSwapIntervalEXT         *swap_interval_ext;
    ProcWGLChoosePixelFormatARB    *choose_pixel_format_arb;

    const char *extensions;
    enum : i32 {
        ContextMajorVersionARB   = 0x2091,
        ContextMinorVersionARB   = 0x2092,
        ContextProfileMaskARB    = 0x9126,
        ContextCoreProfileBitARB = 0x00000001,
        DrawToWindowARB          = 0x2001,
        AccelerationARB          = 0x2003,
        SupportOpenGLARB         = 0x2010,
        DoubleBufferARB          = 0x2011,
        PixelTypeARB             = 0x2013,
        ColorBitsARB             = 0x2014,
        DepthBitsARB             = 0x2022,
        StencilBitsARB           = 0x2023,
        FullAccelerationARB      = 0x2027,
        TypeRGBAARB              = 0x202B,
    };

    bool is_extension_supported(const char *extension) {
        const char *csearch = extension;
        const char *chay = extensions;

        while (*csearch && *chay) {
            if (!(*chay))
                return (!(*csearch));

            if (*csearch == *chay)
                csearch++;
            else
                csearch = extension;

            chay++;
        }

        return (!(*csearch) && !(*chay));
    }
} WGL;

bool create_gl_context(GLContextCreationFormatDesc *desc) {
    // Create a dummy window to query available extensions
    HWND dummy_window;
    HDC dummy_dc;
    WNDCLASSA dummy_window_class = {};

    HWND window = *(HWND*)desc->handle;
    HINSTANCE instance = *(HINSTANCE*)GetWindowLongPtr(window, GWLP_HINSTANCE);

    dummy_window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    dummy_window_class.lpfnWndProc = DefWindowProcA;
    dummy_window_class.hInstance = instance;
    dummy_window_class.lpszClassName = "MokDummyOpenGL";

    if (!RegisterClassA(&dummy_window_class)) {
        return false;
    }

    dummy_window = CreateWindowExA(
        0,
        dummy_window_class.lpszClassName,
        "Mok Dummy Window",
        0,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        0, 0,
        dummy_window_class.hInstance,
        0);

    if (!dummy_window) {
        return false;
    }

    dummy_dc = GetDC(dummy_window);

    PIXELFORMATDESCRIPTOR dummy_pixel_format_desc = {};
    dummy_pixel_format_desc.nSize        = sizeof(dummy_pixel_format_desc);
    dummy_pixel_format_desc.nVersion     = 1;
    dummy_pixel_format_desc.cColorBits   = desc->color_bits;
    dummy_pixel_format_desc.cAlphaBits   = desc->alpha_bits;
    dummy_pixel_format_desc.cDepthBits   = desc->depth_bits;
    dummy_pixel_format_desc.cStencilBits = desc->stencil_bits;
    dummy_pixel_format_desc.iLayerType   = PFD_MAIN_PLANE;

    i32 dummy_pixel_format = ChoosePixelFormat(dummy_dc, &dummy_pixel_format_desc);
    if (dummy_pixel_format == 0) {
        return false;
    }

    if (!SetPixelFormat(dummy_dc, dummy_pixel_format, &dummy_pixel_format_desc)) {
        return false;
    }

    HGLRC dummy_context = wglCreateContext(dummy_dc);
    if (!dummy_context) {
        return false;
    }

    if (!wglMakeCurrent(dummy_dc, dummy_context)) {
        return false;
    }

    WGL.create_context_attribs_arb = (ProcWGLCreateContextAttribsARB*)wglGetProcAddress("wglCreateContextAttribsARB");
    WGL.choose_pixel_format_arb    = (ProcWGLChoosePixelFormatARB*)wglGetProcAddress("wglChoosePixelFormatARB");
    WGL.get_extensions_string_arb  = (ProcWGLGetExtensionsStringARB*)wglGetProcAddress("wglGetExtensionsStringARB");

    if (!WGL.get_extensions_string_arb) {
        return false;
    }

    WGL.extensions = (char*)WGL.get_extensions_string_arb(dummy_dc);

    if (WGL.is_extension_supported("WGL_EXT_swap_control")) {
        desc->swap_interval = true;
        WGL.swap_interval_ext = (ProcWGLSwapIntervalEXT*)wglGetProcAddress("wglSwapIntervalEXT");
    }

    wglMakeCurrent(dummy_dc, 0);
    wglDeleteContext(dummy_context);
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);
    UnregisterClassA(dummy_window_class.lpszClassName, dummy_window_class.hInstance);

    // Now to actually initialize the window
    i32 pixel_format_attribs[] = {
        WGL.DrawToWindowARB,    1,
        WGL.SupportOpenGLARB,   1,
        WGL.DoubleBufferARB,    1,
        WGL.AccelerationARB,    WGL.FullAccelerationARB,
        WGL.PixelTypeARB,       WGL.TypeRGBAARB,
        WGL.ColorBitsARB,       desc->color_bits,
        WGL.DepthBitsARB,       desc->depth_bits,
        WGL.StencilBitsARB,     desc->stencil_bits,
        0
    };


    i32 pixel_format;
    u32 num_formats;
    HGLRC glrc;
    HDC dc;

    dc = GetDC(window);
    WGL.choose_pixel_format_arb(
        dc,
        pixel_format_attribs,
        0, 1,
        &pixel_format, &num_formats);

    if (!num_formats) {
        return false;
    }

    PIXELFORMATDESCRIPTOR pixel_format_desc;
    DescribePixelFormat(dc, pixel_format, sizeof(pixel_format_desc), &pixel_format_desc);
    if (!SetPixelFormat(dc, pixel_format, &pixel_format_desc)) {
        return false;
    }

    i32 profile_flags = 0;
    if (desc->profile_flags & GLProfileFlag::CoreProfile) profile_flags |= WGL.ContextCoreProfileBitARB;

    i32 gl_attribs[] = {
        WGL.ContextMajorVersionARB, desc->major_version,
        WGL.ContextMinorVersionARB, desc->minor_version,
        WGL.ContextProfileMaskARB,  profile_flags,
        0
    };

    glrc = WGL.create_context_attribs_arb(dc, 0, gl_attribs);
    if (!glrc) {
        return false;
    }

    if (!wglMakeCurrent(dc, glrc)) {
        return false;
    }

    return true;
}

bool gl_swap_interval(i32 interval) {
    return WGL.swap_interval_ext(interval);
}

void *gl_get_proc_address(const char *name) {
    return wglGetProcAddress(name);
}

void gl_swap_buffers(void *window_handle) {
    HWND handle = *(HWND*)window_handle;
    static HDC dc = GetDC(handle);
    SwapBuffers(dc);
}

#elif OS_LINUX

#define PROC_GLX_CREATE_CONTEXT_ATTRIBS_ARB(name) GLXContext name(Display *display, GLXFBConfig config, GLXContext parent, Bool b, const int *a)
typedef PROC_GLX_CREATE_CONTEXT_ATTRIBS_ARB(ProcGlxCreateContextAttribsARB);

static struct {
    ProcGlxCreateContextAttribsARB *create_context_attribs_arb;
    GLXContext context;
} GLX;

/**
 * We expect handle to be an array of two pointers:
 *     Display*
 *     Window*
 */

bool create_gl_context(GLContextCreationFormatDesc *desc) {
    umm *params = (umm*)desc->handle;
    Display *display = (Display*)((umm*)desc->handle)[0];
    Window  *window  = (Window*) ((umm*)desc->handle)[1];
    const i32 screen = DefaultScreen(display);

    GLint attribs[] = {
        GLX_X_RENDERABLE,   True,
        GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,    GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
        GLX_RED_SIZE,       desc->color_bits / 3,
        GLX_GREEN_SIZE,     desc->color_bits / 3,
        GLX_BLUE_SIZE,      desc->color_bits / 3,
        GLX_ALPHA_SIZE,     desc->alpha_bits,
        GLX_DEPTH_SIZE,     desc->depth_bits,
        GLX_STENCIL_SIZE,   desc->stencil_bits,
        GLX_DOUBLEBUFFER,   True,
        None
    };

    i32 num_configs;
    GLXFBConfig *configs = glXChooseFBConfig(
        display,
        screen,
        attribs,
        &num_configs);

    if (configs == 0) {
        return false;
    }

    GLXFBConfig config = configs[0];

    XVisualInfo *visual_info = glXGetVisualFromFBConfig(display, config);
    XFree(configs);

    Colormap colormap = XCreateColormap(
        display,
        RootWindow(display, screen),
        visual_info->visual,
        AllocNone);

    XSetWindowAttributes window_attribs;
    window_attribs.background_pixel  = WhitePixel(display, screen);
    window_attribs.border_pixel      = BlackPixel(display, screen);
    window_attribs.override_redirect = True;
    window_attribs.colormap          = colormap;
    window_attribs.event_mask        = ExposureMask;

    XChangeWindowAttributes(
        display,
        *window,
        CWBackPixel | CWColormap | CWBorderPixel | CWEventMask,
        &window_attribs);

    GLX.create_context_attribs_arb = (ProcGlxCreateContextAttribsARB*)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

    i32 flags = 0;

    if (desc->profile_flags & GLProfileFlag::CoreProfile) {
        flags |= GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
    }

    i32 context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, desc->major_version,
        GLX_CONTEXT_MINOR_VERSION_ARB, desc->minor_version,
        GLX_CONTEXT_FLAGS_ARB,         flags,
        None
    };

    GLXContext context = 0;

    const char *glx_extensions = glXQueryExtensionsString(display, screen);
    Str extensions(glx_extensions);

    if (extensions.first_of("GLX_ARB_create_context") != -1) {

        context = glXCreateNewContext(
            display,
            config,
            GLX_RGBA_TYPE,
            0,
            True);

    } else {
        return false;
    }

    XSync(display, False);

    if (!glXIsDirect(display, context)) {
        DEBUG_PRINTF("%s", "GLX context is indirect!");
    }

    glXMakeCurrent(display, *window, context);

    GLX.context = context;

    return true;
}

bool gl_swap_interval(i32 interval) {
    return false;
}

void *gl_get_proc_address(const char *name) {
    return (void*)glXGetProcAddress((const GLubyte*)name);
}

void gl_swap_buffers(void *window_handle) {
    Display *display = (Display*)((umm*)window_handle)[0];
    Window  *window  = (Window*) ((umm*)window_handle)[1];

    glXSwapBuffers(display, *window);
}

#endif // OS_WINDOWS || OS_LINUX



#define GLPROC(rett, name, ...) ProcGL##name gl##name;
GL_ALL_PROCS
#undef GLPROC

bool gl_load_procs() {
    bool result = true;
#define GLPROC(rett, name, ...) \
gl##name = (ProcGL##name)gl_get_proc_address(MSTR(gl##name)); \
result = result && (gl##name != 0);
    GL_ALL_PROCS
#undef GLPROC
    return result;
}


#endif // MOK_COMPAT_OPENGL_IMPLEMENTATION
