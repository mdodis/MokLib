#include "Keycodes.h"

static Str Input_Key_To_String_Table[(u32)InputKey::Count] = {
    LIT("Num0"),    // Num0
    LIT("Num1"),    // Num1
    LIT("Num2"),    // Num2
    LIT("Num3"),    // Num3
    LIT("Num4"),    // Num4
    LIT("Num5"),    // Num5
    LIT("Num6"),    // Num6
    LIT("Num7"),    // Num7
    LIT("Num8"),    // Num8
    LIT("Num9"),    // Num9
    LIT("A"),       // A
    LIT("B"),       // B
    LIT("C"),       // C
    LIT("D"),       // D
    LIT("E"),       // E
    LIT("F"),       // F
    LIT("G"),       // G
    LIT("H"),       // H
    LIT("I"),       // I
    LIT("J"),       // J
    LIT("K"),       // K
    LIT("L"),       // L
    LIT("M"),       // M
    LIT("N"),       // N
    LIT("O"),       // O
    LIT("P"),       // P
    LIT("Q"),       // Q
    LIT("R"),       // R
    LIT("S"),       // S
    LIT("T"),       // T
    LIT("U"),       // U
    LIT("V"),       // V
    LIT("W"),       // W
    LIT("X"),       // X
    LIT("Y"),       // Y
    LIT("Z"),       // Z
    LIT("Space"),   // Space
    LIT("Escape"),  // Escape
};

Str input_key_to_string(InputKey key) {
    return Input_Key_To_String_Table[(u32)key];
}
