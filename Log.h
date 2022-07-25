#pragma once
#include "Config.h"
#include "Tape.h"
#include "Str.h"
#include "StringFormat.h"
#include "FileSystem/FileSystem.h"
#include "RedirectTape.h"

/** Interface for implementing a log */
struct MOKLIB_API ILogContext {
    /** Open the log tape */
    virtual Tape *open() = 0;
    /** Close the tape, flushing any output */
    virtual void close() = 0;
};

/**
 * Simple log to file & console context
 * Uses a single file to log output
 */
struct MOKLIB_API ConsoleLogContext : ILogContext {
    Str filepath;
    FileTape ft;
    StreamTape st;
    RedirectTape<2> tape;

    ConsoleLogContext()
        : tape({&ft, &st})
        {}

    ConsoleLogContext(Str filepath)
        : filepath(filepath)
        , tape({&ft, &st})
        {}

    virtual Tape *open() override;
    virtual void close() override;
};

MOKLIB_API ILogContext* get_log_context();
MOKLIB_API void set_log_context(ILogContext *context);

template <typename First, typename... Rest>
static _inline void format(
    ILogContext *ctx,
    Str fmt_str,
    const First &first,
    const Rest&... rest)
{
    Tape *t = ctx->open();
    format(t, fmt_str, first, rest...);
    format(t, LIT("\n"));
    ctx->close();
}

#define MLOG(fmt, ...) format(get_log_context(), LIT(fmt), __VA_ARGS__)