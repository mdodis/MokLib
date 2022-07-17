#pragma once
#include "Error.h"
#include <errno.h>

static _inline IOError to_io_error(int error_code) {
    switch (error_code) {
        case 0:             return IOError::None;
        case EACCES:        return IOError::AccessDenied;
        default:            return IOError::Unrecognized;
    }
}