#include "Arguments.h"
#include "Parsing.h"

static i32 find_matching_argument_index(
    IArgument **args,
    i32 num_args,
    const Str &id)
{
    for (i32 i = 0; i < num_args; ++i) {
        if (args[i]->name == id) {
            return i;
        }
    }
    return -1;
}

void parse_arguments(const Str &s, IArgument **args, int32 num_args) {
    u64 i = 0;

    i = eat_whitespace(s, i);
    while (i < s.len) {
        if (is_valid_cid_begin(s[i])) {
            Str id;
            i = parse_cid(s, i, id);        // Parse c-style identifier

            int32 arg_index = find_matching_argument_index(args, num_args, id);
            if (arg_index != -1) {

                i = eat_whitespace(s, i);   // Eat whitespace until '='

                if (s[i] != '=') {
                    UNIMPLEMENTED();
                }

                i++;

                // Eat whitespace until we get to the value
                i = eat_whitespace(s, i);

                Str input_str = s.chop_left(i - 1);
                RawTape t(Raw{input_str.data, input_str.len});
                if (!args[arg_index]->parse_argument(&t)) {
                    break;
                }
                i += t.current_offset;
            }

        } else {
            i++;
        }
    }
}
