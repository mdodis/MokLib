#include "Tape.h"

char Tape::read_char() {
    char result;
    if (read(&result, 1) != 1) {
        return EOF;
    } else {
        return result;
    }
}

bool Tape::peek_char(char match) {
    char result;
    if (read(&result, 1) != 1) {
        return false;
    } else {

        if (result == match) {
            return true;
        } else {
            move(-1);
            return false;
        }
    }
}

Str Tape::read_line(IAllocator &alloc) {
    const u32 initial_buffer_size = 1024;

    u32 buffer_size = initial_buffer_size;
    char *data = (char*)alloc.reserve(initial_buffer_size);

    i64 num_read = read(data, initial_buffer_size);
    if (num_read == -1) {
        alloc.release((umm)data);
        return Str::NullStr;
    } else {

        if ((data[num_read - 1]) == '\r' || (data[num_read - 1]) == '\n')
            num_read--;
        if ((data[num_read - 1]) == '\r' || (data[num_read - 1]) == '\n')
            num_read--;

        return Str(data, num_read);
    }
}



u64 SliceTape::get_size() {
    u64 result = 0;
    for (Str &s : strings) {
        result += s.len;
    }
    return result;
}

u64 SliceTape::read(void *destination, u64 amount) {
    u64 num_read = 0;
    while (num_read < amount) {

        if (current_index == strings.count) {
            return 0;
        }

        u64 want_to_read = amount - num_read;
        u64 remaining = strings[current_index].len - current_start_offset;

        u64 bytes_to_read = min(
            remaining,
            want_to_read);

        memcpy(
            (umm)destination + num_read,
            strings[current_index].data + current_start_offset,
            bytes_to_read);
        num_read += bytes_to_read;

        if (bytes_to_read == remaining) {
            current_index++;
            current_start_offset = 0;
        } else {
            current_start_offset += bytes_to_read;
        }

    }

    return num_read;
}

void SliceTape::move(i64 offset) {
    if (offset >= 0)
        move_forwards(offset);
    else
        move_backwards(-offset);
}

void SliceTape::move_forwards(u64 offset) {

    u64 total_moved = 0;
    while (total_moved < offset) {

        if (current_index == strings.count)
            return;

        u64 want_to_move = offset - total_moved;
        u64 remaining = strings[current_index].len - current_start_offset;

        u64 move_amount = min(
            remaining,
            want_to_move);

        if (move_amount == remaining) {
            current_index++;
            current_start_offset = 0;
        } else {
            current_start_offset += move_amount;
        }

        total_moved += move_amount;
    }
}

void SliceTape::move_backwards(u64 offset) {

    u64 total_moved = 0;
    while (total_moved < offset) {
        if ((current_index == 0) && (current_start_offset == 0)) {
            return;
        }

        u64 want_to_move = offset - total_moved;
        u64 remaining = current_start_offset + 1;

        u64 move_amount = min(
            remaining,
            want_to_move);

        if (move_amount == remaining) {
            current_index--;
            current_start_offset = strings[current_index].len - 1;
        } else {
            current_start_offset -= move_amount;
        }

        total_moved += move_amount;
    }

}

bool SliceTape::end() {
    return
        (current_index == strings.count) &&
        (current_start_offset == strings.last()->len);
}

bool SliceTape::write(const void *src, u64 num_bytes) {
    return false;
}

