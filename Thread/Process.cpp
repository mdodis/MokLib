#include "Process.h"

#if OS_LINUX
#include <unistd.h>


bool create_process(Process &process) {
    int result = fork();

    if (result == 0) {
        // child
        constexpr i32 num_max_argv = 16;
        thread_local const char *max_argv[num_max_argv];

        // Parse arguments into argv array
        i32 running_argc = 0;

        max_argv[running_argc++] = process.program;

        const char *c = process.command_line;
        while (*c) {
            max_argv[running_argc++] = c;
            if (running_argc >= (num_max_argv - 1))
                break;

            while (*c && (*c != ' ')) {
                c++;
            }
        }

        max_argv[running_argc] = 0;

        execv(process.program, (char*const*)max_argv);
        return true;
    } else if (result < 0) {
        // error
        return false;
    } else {
        // parent
        process.pid = result;
        return true;
    }
}

#endif // OS_LINUX
