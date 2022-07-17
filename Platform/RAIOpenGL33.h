#pragma once
#include "RAI.h"

#define PROC_RAI_GL_GET_PROC_ADDRESS(name) void *name(const char *pname)
typedef PROC_RAI_GL_GET_PROC_ADDRESS(ProcRAIGLGetProcAddress);

struct RAIGL33InitParams {
    ProcRAIGLGetProcAddress *get_proc;
};

PROC_RAI_INITIALIZE(rai_gl33_init);
