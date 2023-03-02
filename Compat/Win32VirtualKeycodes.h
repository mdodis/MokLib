#include "Platform/Keycodes.h"
#include "Win32Base.h"

static InputKey _WParam_To_InputKey_Table[0xFE] = {
    InputKey::Count,   // 0x0
    InputKey::Count,   // VK_LBUTTON
    InputKey::Count,   // VK_RBUTTON
    InputKey::Count,   // VK_CANCEL
    InputKey::Count,   // VK_MBUTTON
    InputKey::Count,   // VK_XBUTTON1
    InputKey::Count,   // VK_XBUTTON2
    InputKey::Count,   // 0x07
    InputKey::Count,   // VK_BACK
    InputKey::Count,   // VK_TAB
    InputKey::Count,   // 0x0A
    InputKey::Count,   // 0x0B
    InputKey::Count,   // VK_CLEAR
    InputKey::Count,   // VK_RETURN
    InputKey::Count,   // 0x0E
    InputKey::Count,   // 0x0F
    InputKey::Count,   // VK_SHIFT
    InputKey::Count,   // VK_CONTROl
    InputKey::Count,   // VK_MENU
    InputKey::Count,   // VK_PAUSE
    InputKey::Count,   // VK_CAPITAL
    InputKey::Count,   // VK_KANA  | VK_HANGEUL | VK_HANGUL
    InputKey::Count,   // VK_IME_ON
    InputKey::Count,   // VK_JUNJA
    InputKey::Count,   // VK_FINAL
    InputKey::Count,   // VK_HANJA | VK_KANJI
    InputKey::Count,   // VK_IME_OFF
    InputKey::Escape,  // VK_ESCAPE
    InputKey::Count,   // VK_CONVERT
    InputKey::Count,   // VK_NONCONVERT
    InputKey::Count,   // VK_ACCEPT
    InputKey::Count,   // VK_MODECHANGE
    InputKey::Space,   // VK_SPACE
    InputKey::Count,   // VK_PRIOR
    InputKey::Count,   // VK_NEXT
    InputKey::Count,   // VK_END
    InputKey::Count,   // VK_HOME
    InputKey::Count,   // VK_LEFT
    InputKey::Count,   // VK_UP
    InputKey::Count,   // VK_RIGHT
    InputKey::Count,   // VK_DOWN
    InputKey::Count,   // VK_SELECT
    InputKey::Count,   // VK_PRINT
    InputKey::Count,   // VK_EXECUTE
    InputKey::Count,   // VK_SNAPSHOT
    InputKey::Count,   // VK_INSERT
    InputKey::Count,   // VK_DELETE
    InputKey::Count,   // VK_HELP
    /** General Keys */
    InputKey::Num0,   // VK_KEY_0
    InputKey::Num1,   // VK_KEY_1
    InputKey::Num2,   // VK_KEY_2
    InputKey::Num3,   // VK_KEY_3
    InputKey::Num4,   // VK_KEY_4
    InputKey::Num5,   // VK_KEY_5
    InputKey::Num6,   // VK_KEY_6
    InputKey::Num7,   // VK_KEY_7
    InputKey::Num8,   // VK_KEY_8
    InputKey::Num9,   // VK_KEY_9
    InputKey::Num9,   // 0x3A
    InputKey::Count,  // 0x3B
    InputKey::Count,  // 0x3C
    InputKey::Count,  // 0x3D
    InputKey::Count,  // 0x3E
    InputKey::Count,  // 0x3F
    InputKey::Count,  // 0x40
    InputKey::A,      // VK_KEY_A
    InputKey::B,      // VK_KEY_B
    InputKey::C,      // VK_KEY_C
    InputKey::D,      // VK_KEY_D
    InputKey::E,      // VK_KEY_E
    InputKey::F,      // VK_KEY_F
    InputKey::G,      // VK_KEY_G
    InputKey::H,      // VK_KEY_H
    InputKey::I,      // VK_KEY_I
    InputKey::J,      // VK_KEY_J
    InputKey::K,      // VK_KEY_K
    InputKey::L,      // VK_KEY_L
    InputKey::M,      // VK_KEY_M
    InputKey::N,      // VK_KEY_N
    InputKey::O,      // VK_KEY_O
    InputKey::P,      // VK_KEY_P
    InputKey::Q,      // VK_KEY_Q
    InputKey::R,      // VK_KEY_R
    InputKey::S,      // VK_KEY_S
    InputKey::T,      // VK_KEY_T
    InputKey::U,      // VK_KEY_U
    InputKey::V,      // VK_KEY_V
    InputKey::W,      // VK_KEY_W
    InputKey::X,      // VK_KEY_X
    InputKey::Y,      // VK_KEY_Y
    InputKey::Z,      // VK_KEY_Z

    InputKey::Count,  // VK_LWIN
    InputKey::Count,  // VK_RWIN
    InputKey::Count,  // VK_APPS
    InputKey::Count,  // 0x5E
    InputKey::Count,  // VK_SLEEP
    InputKey::Count,  // VK_NUMPAD0
    InputKey::Count,  // VK_NUMPAD1
    InputKey::Count,  // VK_NUMPAD2
    InputKey::Count,  // VK_NUMPAD3
    InputKey::Count,  // VK_NUMPAD4
    InputKey::Count,  // VK_NUMPAD5
    InputKey::Count,  // VK_NUMPAD6
    InputKey::Count,  // VK_NUMPAD7
    InputKey::Count,  // VK_NUMPAD8
    InputKey::Count,  // VK_NUMPAD9
    InputKey::Count,  // VK_MULTIPLY
    InputKey::Count,  // VK_ADD
    InputKey::Count,  // VK_SEPARATOR
    InputKey::Count,  // VK_SUBTRACT
    InputKey::Count,  // VK_DECIMAL
    InputKey::Count,  // VK_DIVIDE
    InputKey::Count,  // VK_F1
    InputKey::Count,  // VK_F2
    InputKey::Count,  // VK_F3
    InputKey::Count,  // VK_F4
    InputKey::Count,  // VK_F5
    InputKey::Count,  // VK_F6
    InputKey::Count,  // VK_F7
    InputKey::Count,  // VK_F8
    InputKey::Count,  // VK_F9
    InputKey::Count,  // VK_F10
    InputKey::Count,  // VK_F11
    InputKey::Count,  // VK_F12
    InputKey::Count,  // VK_F13
    InputKey::Count,  // VK_F14
    InputKey::Count,  // VK_F15
    InputKey::Count,  // VK_F16
    InputKey::Count,  // VK_F17
    InputKey::Count,  // VK_F18
    InputKey::Count,  // VK_F19
    InputKey::Count,  // VK_F20
    InputKey::Count,  // VK_F21
    InputKey::Count,  // VK_F22
    InputKey::Count,  // VK_F23
    InputKey::Count,  // VK_F24
    InputKey::Count,  // VK_NAVIGATION_VIEW    [Reserved]
    InputKey::Count,  // VK_NAVIGATION_MENU    [Reserved]
    InputKey::Count,  // VK_NAVIGATION_UP      [Reserved]
    InputKey::Count,  // VK_NAVIGATION_DOWN    [Reserved]
    InputKey::Count,  // VK_NAVIGATION_LEFT    [Reserved]
    InputKey::Count,  // VK_NAVIGATION_RIGHT   [Reserved]
    InputKey::Count,  // VK_NAVIGATION_ACCEPT  [Reserved]
    InputKey::Count,  // VK_NAVIGATION_CANCEL  [Reserved]
    InputKey::Count,  // VK_NUMLOCK
    InputKey::Count,  // VK_SCROLL
    InputKey::Count,  // VK_OEM_NEC_EQUAL      ['=' key on numpad]
    InputKey::Count,  // VK_OEM_FJ_JISHO
    InputKey::Count,  // VK_OEM_FJ_MASSHOU
    InputKey::Count,  // VK_OEM_FJ_TOUROKU
    InputKey::Count,  // VK_OEM_FJ_LOYA
    InputKey::Count,  // VK_OEM_FJ_ROYA
    InputKey::Count,  // VK_LSHIFT
    InputKey::Count,  // VK_RSHIFT
    InputKey::Count,  // VK_LCONTROL
    InputKey::Count,  // VK_RCONTROL
    InputKey::Count,  // VK_LMENU
    InputKey::Count,  // VK_RMENU
    InputKey::Count,  // VK_BROWSER_BACK
    InputKey::Count,  // VK_BROWSER_FORWARD
    InputKey::Count,  // VK_BROWSER_REFRESH
    InputKey::Count,  // VK_BROWSER_STOP
    InputKey::Count,  // VK_BROWSER_SEARCH
    InputKey::Count,  // VK_BROWSER_FAVORITES
    InputKey::Count,  // VK_BROWSER_HOME
    InputKey::Count,  // VK_VOLUME_MUTE
    InputKey::Count,  // VK_VOLUME_DOWN
    InputKey::Count,  // VK_VOLUME_UP
    InputKey::Count,  // VK_MEDIA_NEXT_TRACK
    InputKey::Count,  // VK_MEDIA_PREV_TRACK
    InputKey::Count,  // VK_MEDIA_STOP
    InputKey::Count,  // VK_MEDIA_PLAY_PAUSE
    InputKey::Count,  // VK_LAUNCH_MAIL
    InputKey::Count,  // VK_LAUNCH_MEDIA_SELECT
    InputKey::Count,  // VK_LAUNCH_APP1
    InputKey::Count,  // VK_LAUNCH_APP2
    InputKey::Count,  // 0xB8
    InputKey::Count,  // 0xB9
    InputKey::Count,  // VK_OEM_1       ';:' for US
    InputKey::Count,  // VK_OEM_PLUS    '+' any country
    InputKey::Count,  // VK_OEM_COMMA   ',' any country
    InputKey::Count,  // VK_OEM_MINUS   '-' any country
    InputKey::Count,  // VK_OEM_PERIOD  '.' any country
    InputKey::Count,  // VK_OEM_2       '/?' for US
    InputKey::Count,  // VK_OEM_3       '`~' for US
    InputKey::Count,  // 0xC1
    InputKey::Count,  // 0xC2
    InputKey::Count,  // VK_GAMEPAD_A                       [Reserved]
    InputKey::Count,  // VK_GAMEPAD_B                       [Reserved]
    InputKey::Count,  // VK_GAMEPAD_X                       [Reserved]
    InputKey::Count,  // VK_GAMEPAD_Y                       [Reserved]
    InputKey::Count,  // VK_GAMEPAD_RIGHT_SHOULDER          [Reserved]
    InputKey::Count,  // VK_GAMEPAD_LEFT_SHOULDER           [Reserved]
    InputKey::Count,  // VK_GAMEPAD_LEFT_TRIGGER            [Reserved]
    InputKey::Count,  // VK_GAMEPAD_RIGHT_TRIGGER           [Reserved]
    InputKey::Count,  // VK_GAMEPAD_DPAD_UP                 [Reserved]
    InputKey::Count,  // VK_GAMEPAD_DPAD_DOWN               [Reserved]
    InputKey::Count,  // VK_GAMEPAD_DPAD_LEFT               [Reserved]
    InputKey::Count,  // VK_GAMEPAD_DPAD_RIGHT              [Reserved]
    InputKey::Count,  // VK_GAMEPAD_MENU                    [Reserved]
    InputKey::Count,  // VK_GAMEPAD_VIEW                    [Reserved]
    InputKey::Count,  // VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON  [Reserved]
    InputKey::Count,  // VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON [Reserved]
    InputKey::Count,  // VK_GAMEPAD_LEFT_THUMBSTICK_UP      [Reserved]
    InputKey::Count,  // VK_GAMEPAD_LEFT_THUMBSTICK_DOWN    [Reserved]
    InputKey::Count,  // VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT   [Reserved]
    InputKey::Count,  // VK_GAMEPAD_LEFT_THUMBSTICK_LEFT    [Reserved]
    InputKey::Count,  // VK_GAMEPAD_RIGHT_THUMBSTICK_UP     [Reserved]
    InputKey::Count,  // VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN   [Reserved]
    InputKey::Count,  // VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT  [Reserved]
    InputKey::Count,  // VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT   [Reserved]
    InputKey::Count,  // VK_OEM_4 '[{' for US
    InputKey::Count,  // VK_OEM_5 '\|' for US
    InputKey::Count,  // VK_OEM_6 ']}' for US
    InputKey::Count,  // VK_OEM_7 ''"' for US
    InputKey::Count,  // VK_OEM_8
    InputKey::Count,  // 0xE0
    InputKey::Count,  // VK_OEM_AX    'AX' key on Japanese AX kbd
    InputKey::Count,  // VK_OEM_102   "<>" or "\|" on RT 102-key kbd.
    InputKey::Count,  // VK_ICO_HELP  Help key on ICO
    InputKey::Count,  // VK_ICO_00    00 key on ICO
    InputKey::Count,  // VK_PROCESSKEY
    InputKey::Count,  // VK_ICO_CLEAR
    InputKey::Count,  // VK_PACKET
    InputKey::Count,  // 0xE8
    InputKey::Count,  // VK_OEM_RESET   [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_JUMP    [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_PA1     [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_PA2     [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_PA3     [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_WSCTRL  [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_CUSEL   [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_ATTN    [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_FINISH  [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_COPY    [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_AUTO    [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_ENLW    [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_BACKTAB [Nokia/Ericsson]
    InputKey::Count,  // VK_ATTN        [Nokia/Ericsson]
    InputKey::Count,  // VK_CRSEL       [Nokia/Ericsson]
    InputKey::Count,  // VK_EXSEL       [Nokia/Ericsson]
    InputKey::Count,  // VK_EREOF       [Nokia/Ericsson]
    InputKey::Count,  // VK_PLAY        [Nokia/Ericsson]
    InputKey::Count,  // VK_ZOOM        [Nokia/Ericsson]
    InputKey::Count,  // VK_NONAME      [Nokia/Ericsson]
    InputKey::Count,  // VK_PA1         [Nokia/Ericsson]
    InputKey::Count,  // VK_OEM_CLEAR   [Nokia/Ericsson]
};

static _inline InputKey wparam_to_input_key(Win32::WPARAM wparam)
{
    return _WParam_To_InputKey_Table[wparam];
}
