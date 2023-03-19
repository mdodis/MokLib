#pragma once
#ifndef MOK_GLFW_VIRTUAL_KEYCODES_AUTO_INCLUDE
#define MOK_GLFW_VIRTUAL_KEYCODES_AUTO_INCLUDE 1
#endif
#if MOK_GLFW_VIRTUAL_KEYCODES_AUTO_INCLUDE
#include <GLFW/glfw3.h>
#endif
#include "Platform/Keycodes.h"

static InputKey _GLFW_Key_To_InputKey_Table[GLFW_KEY_LAST + 1] = {
    InputKey::Count,                 // 0   Unknown or invalid key
    InputKey::Count,                 // 1   Unknown or invalid key
    InputKey::Count,                 // 2   Unknown or invalid key
    InputKey::Count,                 // 3   Unknown or invalid key
    InputKey::Count,                 // 4   Unknown or invalid key
    InputKey::Count,                 // 5   Unknown or invalid key
    InputKey::Count,                 // 6   Unknown or invalid key
    InputKey::Count,                 // 7   Unknown or invalid key
    InputKey::Count,                 // 8   Unknown or invalid key
    InputKey::Count,                 // 9   Unknown or invalid key
    InputKey::Count,                 // 10  Unknown or invalid key
    InputKey::Count,                 // 11  Unknown or invalid key
    InputKey::Count,                 // 12  Unknown or invalid key
    InputKey::Count,                 // 13  Unknown or invalid key
    InputKey::Count,                 // 14  Unknown or invalid key
    InputKey::Count,                 // 15  Unknown or invalid key
    InputKey::Count,                 // 16  Unknown or invalid key
    InputKey::Count,                 // 17  Unknown or invalid key
    InputKey::Count,                 // 18  Unknown or invalid key
    InputKey::Count,                 // 19  Unknown or invalid key
    InputKey::Count,                 // 20  Unknown or invalid key
    InputKey::Count,                 // 21  Unknown or invalid key
    InputKey::Count,                 // 22  Unknown or invalid key
    InputKey::Count,                 // 23  Unknown or invalid key
    InputKey::Count,                 // 24  Unknown or invalid key
    InputKey::Count,                 // 25  Unknown or invalid key
    InputKey::Count,                 // 26  Unknown or invalid key
    InputKey::Count,                 // 27  Unknown or invalid key
    InputKey::Count,                 // 28  Unknown or invalid key
    InputKey::Count,                 // 29  Unknown or invalid key
    InputKey::Count,                 // 30  Unknown or invalid key
    InputKey::Count,                 // 31  Unknown or invalid key
    InputKey::Space,                 // 32  GLFW_KEY_SPACE
    InputKey::Count,                 // 33  Unknown or invalid key
    InputKey::Count,                 // 34  Unknown or invalid key
    InputKey::Count,                 // 35  Unknown or invalid key
    InputKey::Count,                 // 36  Unknown or invalid key
    InputKey::Count,                 // 37  Unknown or invalid key
    InputKey::Count,                 // 38  Unknown or invalid key
    InputKey::Count,                 // 39  GLFW_KEY_APOSTROPHE
    InputKey::Count,                 // 40  Unknown or invalid key
    InputKey::Count,                 // 41  Unknown or invalid key
    InputKey::Count,                 // 42  Unknown or invalid key
    InputKey::Count,                 // 43  Unknown or invalid key
    InputKey::Count,                 // 44  GLFW_KEY_COMMA
    InputKey::Count,                 // 45  GLFW_KEY_MINUS
    InputKey::Count,                 // 46  GLFW_KEY_PERIOD
    InputKey::Count,                 // 47  GLFW_KEY_SLASH
    InputKey::Num0,                  // 48  GLFW_KEY_0
    InputKey::Num1,                  // 49  GLFW_KEY_1
    InputKey::Num2,                  // 50  GLFW_KEY_2
    InputKey::Num3,                  // 51  GLFW_KEY_3
    InputKey::Num4,                  // 52  GLFW_KEY_4
    InputKey::Num5,                  // 53  GLFW_KEY_5
    InputKey::Num6,                  // 54  GLFW_KEY_6
    InputKey::Num7,                  // 55  GLFW_KEY_7
    InputKey::Num8,                  // 56  GLFW_KEY_8
    InputKey::Num9,                  // 57  GLFW_KEY_9
    InputKey::Count,                 // 58  Unknown or invalid key
    InputKey::Count,                 // 59  GLFW_KEY_SEMICOLON
    InputKey::Count,                 // 60  Unknown or invalid key
    InputKey::Count,                 // 61  GLFW_KEY_EQUAL
    InputKey::Count,                 // 62  Unknown or invalid key
    InputKey::Count,                 // 63  Unknown or invalid key
    InputKey::Count,                 // 64  Unknown or invalid key
    InputKey::A,                     // 65  GLFW_KEY_A
    InputKey::B,                     // 66  GLFW_KEY_B
    InputKey::C,                     // 67  GLFW_KEY_C
    InputKey::D,                     // 68  GLFW_KEY_D
    InputKey::E,                     // 69  GLFW_KEY_E
    InputKey::F,                     // 70  GLFW_KEY_F
    InputKey::G,                     // 71  GLFW_KEY_G
    InputKey::H,                     // 72  GLFW_KEY_H
    InputKey::I,                     // 73  GLFW_KEY_I
    InputKey::J,                     // 74  GLFW_KEY_J
    InputKey::K,                     // 75  GLFW_KEY_K
    InputKey::L,                     // 76  GLFW_KEY_L
    InputKey::M,                     // 77  GLFW_KEY_M
    InputKey::N,                     // 78  GLFW_KEY_N
    InputKey::O,                     // 79  GLFW_KEY_O
    InputKey::P,                     // 80  GLFW_KEY_P
    InputKey::Q,                     // 81  GLFW_KEY_Q
    InputKey::R,                     // 82  GLFW_KEY_R
    InputKey::S,                     // 83  GLFW_KEY_S
    InputKey::T,                     // 84  GLFW_KEY_T
    InputKey::U,                     // 85  GLFW_KEY_U
    InputKey::V,                     // 86  GLFW_KEY_V
    InputKey::W,                     // 87  GLFW_KEY_W
    InputKey::X,                     // 88  GLFW_KEY_X
    InputKey::Y,                     // 89  GLFW_KEY_Y
    InputKey::Z,                     // 90  GLFW_KEY_Z
    InputKey::Count,                 // 91  GLFW_KEY_LEFT_BRACKET
    InputKey::Count,                 // 92  GLFW_KEY_BACKSLASH
    InputKey::Count,                 // 93  GLFW_KEY_RIGHT_BRACKET
    InputKey::Count,                 // 94  Unknown or invalid key
    InputKey::Count,                 // 95  Unknown or invalid key
    InputKey::Count,                 // 96  GLFW_KEY_GRAVE_ACCENT
    InputKey::Count,                 // 97  Unknown or invalid key
    InputKey::Count,                 // 98  Unknown or invalid key
    InputKey::Count,                 // 99  Unknown or invalid key
    InputKey::Count,                 // 100 Unknown or invalid key
    InputKey::Count,                 // 101 Unknown or invalid key
    InputKey::Count,                 // 102 Unknown or invalid key
    InputKey::Count,                 // 103 Unknown or invalid key
    InputKey::Count,                 // 104 Unknown or invalid key
    InputKey::Count,                 // 105 Unknown or invalid key
    InputKey::Count,                 // 106 Unknown or invalid key
    InputKey::Count,                 // 107 Unknown or invalid key
    InputKey::Count,                 // 108 Unknown or invalid key
    InputKey::Count,                 // 109 Unknown or invalid key
    InputKey::Count,                 // 110 Unknown or invalid key
    InputKey::Count,                 // 111 Unknown or invalid key
    InputKey::Count,                 // 112 Unknown or invalid key
    InputKey::Count,                 // 113 Unknown or invalid key
    InputKey::Count,                 // 114 Unknown or invalid key
    InputKey::Count,                 // 115 Unknown or invalid key
    InputKey::Count,                 // 116 Unknown or invalid key
    InputKey::Count,                 // 117 Unknown or invalid key
    InputKey::Count,                 // 118 Unknown or invalid key
    InputKey::Count,                 // 119 Unknown or invalid key
    InputKey::Count,                 // 120 Unknown or invalid key
    InputKey::Count,                 // 121 Unknown or invalid key
    InputKey::Count,                 // 122 Unknown or invalid key
    InputKey::Count,                 // 123 Unknown or invalid key
    InputKey::Count,                 // 124 Unknown or invalid key
    InputKey::Count,                 // 125 Unknown or invalid key
    InputKey::Count,                 // 126 Unknown or invalid key
    InputKey::Count,                 // 127 Unknown or invalid key
    InputKey::Count,                 // 128 Unknown or invalid key
    InputKey::Count,                 // 129 Unknown or invalid key
    InputKey::Count,                 // 130 Unknown or invalid key
    InputKey::Count,                 // 131 Unknown or invalid key
    InputKey::Count,                 // 132 Unknown or invalid key
    InputKey::Count,                 // 133 Unknown or invalid key
    InputKey::Count,                 // 134 Unknown or invalid key
    InputKey::Count,                 // 135 Unknown or invalid key
    InputKey::Count,                 // 136 Unknown or invalid key
    InputKey::Count,                 // 137 Unknown or invalid key
    InputKey::Count,                 // 138 Unknown or invalid key
    InputKey::Count,                 // 139 Unknown or invalid key
    InputKey::Count,                 // 140 Unknown or invalid key
    InputKey::Count,                 // 141 Unknown or invalid key
    InputKey::Count,                 // 142 Unknown or invalid key
    InputKey::Count,                 // 143 Unknown or invalid key
    InputKey::Count,                 // 144 Unknown or invalid key
    InputKey::Count,                 // 145 Unknown or invalid key
    InputKey::Count,                 // 146 Unknown or invalid key
    InputKey::Count,                 // 147 Unknown or invalid key
    InputKey::Count,                 // 148 Unknown or invalid key
    InputKey::Count,                 // 149 Unknown or invalid key
    InputKey::Count,                 // 150 Unknown or invalid key
    InputKey::Count,                 // 151 Unknown or invalid key
    InputKey::Count,                 // 152 Unknown or invalid key
    InputKey::Count,                 // 153 Unknown or invalid key
    InputKey::Count,                 // 154 Unknown or invalid key
    InputKey::Count,                 // 155 Unknown or invalid key
    InputKey::Count,                 // 156 Unknown or invalid key
    InputKey::Count,                 // 157 Unknown or invalid key
    InputKey::Count,                 // 158 Unknown or invalid key
    InputKey::Count,                 // 159 Unknown or invalid key
    InputKey::Count,                 // 160 Unknown or invalid key
    InputKey::Count,                 // 161 GLFW_KEY_WORLD_1
    InputKey::Count,                 // 162 GLFW_KEY_WORLD_2
    InputKey::Count,                 // 163 Unknown or invalid key
    InputKey::Count,                 // 164 Unknown or invalid key
    InputKey::Count,                 // 165 Unknown or invalid key
    InputKey::Count,                 // 166 Unknown or invalid key
    InputKey::Count,                 // 167 Unknown or invalid key
    InputKey::Count,                 // 168 Unknown or invalid key
    InputKey::Count,                 // 169 Unknown or invalid key
    InputKey::Count,                 // 170 Unknown or invalid key
    InputKey::Count,                 // 171 Unknown or invalid key
    InputKey::Count,                 // 172 Unknown or invalid key
    InputKey::Count,                 // 173 Unknown or invalid key
    InputKey::Count,                 // 174 Unknown or invalid key
    InputKey::Count,                 // 175 Unknown or invalid key
    InputKey::Count,                 // 176 Unknown or invalid key
    InputKey::Count,                 // 177 Unknown or invalid key
    InputKey::Count,                 // 178 Unknown or invalid key
    InputKey::Count,                 // 179 Unknown or invalid key
    InputKey::Count,                 // 180 Unknown or invalid key
    InputKey::Count,                 // 181 Unknown or invalid key
    InputKey::Count,                 // 182 Unknown or invalid key
    InputKey::Count,                 // 183 Unknown or invalid key
    InputKey::Count,                 // 184 Unknown or invalid key
    InputKey::Count,                 // 185 Unknown or invalid key
    InputKey::Count,                 // 186 Unknown or invalid key
    InputKey::Count,                 // 187 Unknown or invalid key
    InputKey::Count,                 // 188 Unknown or invalid key
    InputKey::Count,                 // 189 Unknown or invalid key
    InputKey::Count,                 // 190 Unknown or invalid key
    InputKey::Count,                 // 191 Unknown or invalid key
    InputKey::Count,                 // 192 Unknown or invalid key
    InputKey::Count,                 // 193 Unknown or invalid key
    InputKey::Count,                 // 194 Unknown or invalid key
    InputKey::Count,                 // 195 Unknown or invalid key
    InputKey::Count,                 // 196 Unknown or invalid key
    InputKey::Count,                 // 197 Unknown or invalid key
    InputKey::Count,                 // 198 Unknown or invalid key
    InputKey::Count,                 // 199 Unknown or invalid key
    InputKey::Count,                 // 200 Unknown or invalid key
    InputKey::Count,                 // 201 Unknown or invalid key
    InputKey::Count,                 // 202 Unknown or invalid key
    InputKey::Count,                 // 203 Unknown or invalid key
    InputKey::Count,                 // 204 Unknown or invalid key
    InputKey::Count,                 // 205 Unknown or invalid key
    InputKey::Count,                 // 206 Unknown or invalid key
    InputKey::Count,                 // 207 Unknown or invalid key
    InputKey::Count,                 // 208 Unknown or invalid key
    InputKey::Count,                 // 209 Unknown or invalid key
    InputKey::Count,                 // 210 Unknown or invalid key
    InputKey::Count,                 // 211 Unknown or invalid key
    InputKey::Count,                 // 212 Unknown or invalid key
    InputKey::Count,                 // 213 Unknown or invalid key
    InputKey::Count,                 // 214 Unknown or invalid key
    InputKey::Count,                 // 215 Unknown or invalid key
    InputKey::Count,                 // 216 Unknown or invalid key
    InputKey::Count,                 // 217 Unknown or invalid key
    InputKey::Count,                 // 218 Unknown or invalid key
    InputKey::Count,                 // 219 Unknown or invalid key
    InputKey::Count,                 // 220 Unknown or invalid key
    InputKey::Count,                 // 221 Unknown or invalid key
    InputKey::Count,                 // 222 Unknown or invalid key
    InputKey::Count,                 // 223 Unknown or invalid key
    InputKey::Count,                 // 224 Unknown or invalid key
    InputKey::Count,                 // 225 Unknown or invalid key
    InputKey::Count,                 // 226 Unknown or invalid key
    InputKey::Count,                 // 227 Unknown or invalid key
    InputKey::Count,                 // 228 Unknown or invalid key
    InputKey::Count,                 // 229 Unknown or invalid key
    InputKey::Count,                 // 230 Unknown or invalid key
    InputKey::Count,                 // 231 Unknown or invalid key
    InputKey::Count,                 // 232 Unknown or invalid key
    InputKey::Count,                 // 233 Unknown or invalid key
    InputKey::Count,                 // 234 Unknown or invalid key
    InputKey::Count,                 // 235 Unknown or invalid key
    InputKey::Count,                 // 236 Unknown or invalid key
    InputKey::Count,                 // 237 Unknown or invalid key
    InputKey::Count,                 // 238 Unknown or invalid key
    InputKey::Count,                 // 239 Unknown or invalid key
    InputKey::Count,                 // 240 Unknown or invalid key
    InputKey::Count,                 // 241 Unknown or invalid key
    InputKey::Count,                 // 242 Unknown or invalid key
    InputKey::Count,                 // 243 Unknown or invalid key
    InputKey::Count,                 // 244 Unknown or invalid key
    InputKey::Count,                 // 245 Unknown or invalid key
    InputKey::Count,                 // 246 Unknown or invalid key
    InputKey::Count,                 // 247 Unknown or invalid key
    InputKey::Count,                 // 248 Unknown or invalid key
    InputKey::Count,                 // 249 Unknown or invalid key
    InputKey::Count,                 // 250 Unknown or invalid key
    InputKey::Count,                 // 251 Unknown or invalid key
    InputKey::Count,                 // 252 Unknown or invalid key
    InputKey::Count,                 // 253 Unknown or invalid key
    InputKey::Count,                 // 254 Unknown or invalid key
    InputKey::Count,                 // 255 Unknown or invalid key
    InputKey::Escape,                // 256 GLFW_KEY_ESCAPE
    InputKey::Count,                 // 257 GLFW_KEY_ENTER
    InputKey::Count,                 // 258 GLFW_KEY_TAB
    InputKey::Count,                 // 259 GLFW_KEY_BACKSPACE
    InputKey::Count,                 // 260 GLFW_KEY_INSERT
    InputKey::Count,                 // 261 GLFW_KEY_DELETE
    InputKey::Count,                 // 262 GLFW_KEY_RIGHT
    InputKey::Count,                 // 263 GLFW_KEY_LEFT
    InputKey::Count,                 // 264 GLFW_KEY_DOWN
    InputKey::Count,                 // 265 GLFW_KEY_UP
    InputKey::Count,                 // 266 GLFW_KEY_PAGE_UP
    InputKey::Count,                 // 267 GLFW_KEY_PAGE_DOWN
    InputKey::Count,                 // 268 GLFW_KEY_HOME
    InputKey::Count,                 // 269 GLFW_KEY_END
    InputKey::Count,                 // 270 Unknown or invalid key
    InputKey::Count,                 // 271 Unknown or invalid key
    InputKey::Count,                 // 272 Unknown or invalid key
    InputKey::Count,                 // 273 Unknown or invalid key
    InputKey::Count,                 // 274 Unknown or invalid key
    InputKey::Count,                 // 275 Unknown or invalid key
    InputKey::Count,                 // 276 Unknown or invalid key
    InputKey::Count,                 // 277 Unknown or invalid key
    InputKey::Count,                 // 278 Unknown or invalid key
    InputKey::Count,                 // 279 Unknown or invalid key
    InputKey::Count,                 // 280 GLFW_KEY_CAPS_LOCK
    InputKey::Count,                 // 281 GLFW_KEY_SCROLL_LOCK
    InputKey::Count,                 // 282 GLFW_KEY_NUM_LOCK
    InputKey::Count,                 // 283 GLFW_KEY_PRINT_SCREEN
    InputKey::Count,                 // 284 GLFW_KEY_PAUSE
    InputKey::Count,                 // 285 Unknown or invalid key
    InputKey::Count,                 // 286 Unknown or invalid key
    InputKey::Count,                 // 287 Unknown or invalid key
    InputKey::Count,                 // 288 Unknown or invalid key
    InputKey::Count,                 // 289 Unknown or invalid key
    InputKey::Count,                 // 290 GLFW_KEY_F1
    InputKey::Count,                 // 291 GLFW_KEY_F2
    InputKey::Count,                 // 292 GLFW_KEY_F3
    InputKey::Count,                 // 293 GLFW_KEY_F4
    InputKey::Count,                 // 294 GLFW_KEY_F5
    InputKey::Count,                 // 295 GLFW_KEY_F6
    InputKey::Count,                 // 296 GLFW_KEY_F7
    InputKey::Count,                 // 297 GLFW_KEY_F8
    InputKey::Count,                 // 298 GLFW_KEY_F9
    InputKey::Count,                 // 299 GLFW_KEY_F10
    InputKey::Count,                 // 300 GLFW_KEY_F11
    InputKey::Count,                 // 301 GLFW_KEY_F12
    InputKey::Count,                 // 302 GLFW_KEY_F13
    InputKey::Count,                 // 303 GLFW_KEY_F14
    InputKey::Count,                 // 304 GLFW_KEY_F15
    InputKey::Count,                 // 305 GLFW_KEY_F16
    InputKey::Count,                 // 306 GLFW_KEY_F17
    InputKey::Count,                 // 307 GLFW_KEY_F18
    InputKey::Count,                 // 308 GLFW_KEY_F19
    InputKey::Count,                 // 309 GLFW_KEY_F20
    InputKey::Count,                 // 310 GLFW_KEY_F21
    InputKey::Count,                 // 311 GLFW_KEY_F22
    InputKey::Count,                 // 312 GLFW_KEY_F23
    InputKey::Count,                 // 313 GLFW_KEY_F24
    InputKey::Count,                 // 314 GLFW_KEY_F25
    InputKey::Count,                 // 315 Unknown or invalid key
    InputKey::Count,                 // 316 Unknown or invalid key
    InputKey::Count,                 // 317 Unknown or invalid key
    InputKey::Count,                 // 318 Unknown or invalid key
    InputKey::Count,                 // 319 Unknown or invalid key
    InputKey::Count,                 // 320 GLFW_KEY_KP_0
    InputKey::Count,                 // 321 GLFW_KEY_KP_1
    InputKey::Count,                 // 322 GLFW_KEY_KP_2
    InputKey::Count,                 // 323 GLFW_KEY_KP_3
    InputKey::Count,                 // 324 GLFW_KEY_KP_4
    InputKey::Count,                 // 325 GLFW_KEY_KP_5
    InputKey::Count,                 // 326 GLFW_KEY_KP_6
    InputKey::Count,                 // 327 GLFW_KEY_KP_7
    InputKey::Count,                 // 328 GLFW_KEY_KP_8
    InputKey::Count,                 // 329 GLFW_KEY_KP_9
    InputKey::Count,                 // 330 GLFW_KEY_KP_DECIMAL
    InputKey::Count,                 // 331 GLFW_KEY_KP_DIVIDE
    InputKey::Count,                 // 332 GLFW_KEY_KP_MULTIPLY
    InputKey::Count,                 // 333 GLFW_KEY_KP_SUBTRACT
    InputKey::Count,                 // 334 GLFW_KEY_KP_ADD
    InputKey::Count,                 // 335 GLFW_KEY_KP_ENTER
    InputKey::Count,                 // 336 GLFW_KEY_KP_EQUAL
    InputKey::Count,                 // 337 Unknown or invalid key
    InputKey::Count,                 // 338 Unknown or invalid key
    InputKey::Count,                 // 339 Unknown or invalid key
    InputKey::Count,                 // 340 GLFW_KEY_LEFT_SHIFT
    InputKey::Count,                 // 341 GLFW_KEY_LEFT_CONTROL
    InputKey::Count,                 // 342 GLFW_KEY_LEFT_ALT
    InputKey::Count,                 // 343 GLFW_KEY_LEFT_SUPER
    InputKey::Count,                 // 344 GLFW_KEY_RIGHT_SHIFT
    InputKey::Count,                 // 345 GLFW_KEY_RIGHT_CONTROL
    InputKey::Count,                 // 346 GLFW_KEY_RIGHT_ALT
    InputKey::Count,                 // 347 GLFW_KEY_RIGHT_SUPER
    InputKey::Count,                 // 348 GLFW_KEY_MENU
};

static _inline InputKey glfw_key_to_input_key(int key)
{
    return _GLFW_Key_To_InputKey_Table[key];
}
